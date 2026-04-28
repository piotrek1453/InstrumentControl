#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: $0 [-p <serial port>] [-m <mount point>] [-f <output logs filename>] [-r <flash method: uf2|cmsis-dap>]" >&2
}

serial_port=""
mount_point=""
logs_output_file=""
flash_method="uf2"
OPENOCD_ADAPTER_SPEED="1000"

while getopts ":p:m:f:r:h" o; do
  case "${o}" in
  p) serial_port=${OPTARG} ;;
  m) mount_point=${OPTARG} ;;
  f) logs_output_file=${OPTARG} ;;
  r) flash_method=${OPTARG} ;;
  h) usage; exit 0 ;;
  *) usage; exit 1 ;;
  esac
done

# detect picotool: prefer system `picotool`, fall back to Pico SDK bundled picotool
PICOTOOL=""
if command -v picotool >/dev/null 2>&1; then
  PICOTOOL="$(command -v picotool)"
else
  # look under ~/.pico-sdk/picotool/*/picotool/picotool
  for p in "$HOME"/.pico-sdk/picotool/*/picotool/picotool; do
    if [[ -x "$p" ]]; then
      PICOTOOL="$p"
      break
    fi
  done
fi


script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$script_dir"
build_dir="$project_dir/build"

on_interrupt() {
  trap - INT TERM
  kill 0 2>/dev/null || true
  exit 130
}
trap on_interrupt INT TERM

wait_for_serial_port() {
  local port="$1"
  local timeout_seconds="${2:-10}"
  local deadline=$((SECONDS + timeout_seconds))

  while [[ $SECONDS -lt $deadline ]]; do
    if [[ -e "$port" ]]; then
      return 0
    fi
    sleep 0.2
  done

  return 1
}

echo "Building RP2040 project in $project_dir"
if [[ -d "$build_dir" ]]; then
  cmake --build "$build_dir"
else
  cmake -S "$project_dir" -B "$build_dir"
  cmake --build "$build_dir"
fi

elf_file="$build_dir/RP2040.elf"
uf2_file="$build_dir/RP2040.uf2"

if [[ "$flash_method" == "cmsis-dap" ]]; then
  if [[ ! -f "$elf_file" ]]; then
    echo "Build succeeded but ELF not found: $elf_file" >&2
    exit 1
  fi
  if command -v openocd >/dev/null 2>&1; then
    echo "Flashing via CMSIS-DAP (OpenOCD) using $elf_file (adapter speed ${OPENOCD_ADAPTER_SPEED} kHz)"
    openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
      -c "adapter speed ${OPENOCD_ADAPTER_SPEED}" \
      -c "init" \
      -c "reset init" \
      -c "program $elf_file verify" \
      -c "reset run" \
      -c "exit"
  else
    echo "openocd not found. Install OpenOCD to flash via CMSIS-DAP." >&2
    exit 1
  fi
else
  if [[ ! -f "$uf2_file" ]]; then
    echo "Build succeeded but UF2 not found: $uf2_file" >&2
    exit 1
  fi

  if [[ -n "$mount_point" ]]; then
    if [[ ! -d "$mount_point" ]]; then
      echo "Mount point not found: $mount_point" >&2
      exit 1
    fi
    echo "Copying $uf2_file to $mount_point"
    cp "$uf2_file" "$mount_point/"
  else
    if [[ -n "$PICOTOOL" ]]; then
      echo "Flashing using $PICOTOOL"
      "$PICOTOOL" load "$uf2_file"
    else
      echo "No mount point provided and 'picotool' not found." >&2
      echo "Please mount the Pico (BOOTSEL) and copy $uf2_file to it, or install picotool." >&2
    fi
  fi
fi

if [[ -n "$serial_port" ]]; then
  if ! wait_for_serial_port "$serial_port" 15; then
    echo "Serial port not available after flash: $serial_port" >&2
    exit 1
  fi

  if python3 -m serial.tools.miniterm -h >/dev/null 2>&1; then
    monitor_cmd=(python3 -m serial.tools.miniterm "$serial_port" 115200 --eol CRLF --raw)
  elif command -v screen >/dev/null 2>&1; then
    monitor_cmd=(screen "$serial_port" 115200)
  else
    echo "No serial monitor found (python3 pyserial miniterm/screen). Install one or provide logs file." >&2
    exit 1
  fi

  if [[ -n "$logs_output_file" ]]; then
    mkdir -p "$(dirname "$logs_output_file")"
    "${monitor_cmd[@]}" 2>&1 | tee "$logs_output_file"
    monitor_exit_code=${PIPESTATUS[0]}
    exit "$monitor_exit_code"
  else
    "${monitor_cmd[@]}"
  fi
else
  echo "Flashed $uf2_file. No serial port provided; not starting monitor."
fi

exit 0
