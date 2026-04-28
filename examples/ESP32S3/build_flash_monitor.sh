#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: $0 -p <ESP32 port> [-f <output logs filename>]" >&2
}

esp32_port=""
logs_output_file=""

while getopts ":p:f:h" o; do
  case "${o}" in
  f) logs_output_file=${OPTARG} ;;
  p) esp32_port=${OPTARG} ;;
  h) usage; exit 0 ;;
  *) usage; exit 1 ;;
  esac
done

if [[ -z "${esp32_port}" ]]; then
  usage
  exit 1
fi

export ESP32_PORT="$esp32_port"
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export ESP_IDF_MONITOR_CFGFILE="$script_dir/esp-idf-monitor.cfg"

idf_env_output="$($HOME/.espressif/tools/activate_idf_v5.5.3.sh -e)"
idf_path_prefix=""

while IFS= read -r env_line; do
  case "$env_line" in
  PATH=*) idf_path_prefix="${env_line#PATH=}" ;;
  SYSTEM_PATH=*) export SYSTEM_PATH="${env_line#SYSTEM_PATH=}" ;;
  *=*) export "$env_line" ;;
  esac
done <<< "$idf_env_output"

if [[ -n "$idf_path_prefix" ]]; then
  export PATH="$idf_path_prefix:$PATH"
fi

if [[ -z "${IDF_PATH:-}" ]]; then
  echo "Failed to activate ESP-IDF environment" >&2
  exit 1
fi

on_interrupt() {
  trap - INT TERM
  kill 0 2>/dev/null || true
  exit 130
}

trap on_interrupt INT TERM

"$IDF_PATH/tools/idf.py" fullclean
"$IDF_PATH/tools/idf.py" build
"$IDF_PATH/tools/idf.py" -p "$ESP32_PORT" flash

monitor_command=("$IDF_PATH/tools/idf.py" -p "$ESP32_PORT" monitor)

if [[ -n "${logs_output_file}" ]]; then
  mkdir -p "$(dirname "$logs_output_file")"
  "${monitor_command[@]}" 2>&1 | tee "$logs_output_file"
  monitor_exit_code=${PIPESTATUS[0]}
  exit "$monitor_exit_code"
else
  "${monitor_command[@]}"
fi
