#!/usr/bin/env bash

set -e

while getopts ":p:f:" o; do
  case "${o}" in
  f) logs_output_file=${OPTARG} ;;
  p) esp32_port=${OPTARG} ;;
  *) echo "Usage: [-p <ESP32 port>] [-f <output logs filename>]" ;;
  esac
done

# some voodoo so that ESP_IDF activates correctly
export ESP32_PORT="$esp32_port"

bash << 'SCRIPT'
source "$HOME/.espressif/tools/activate_idf_v5.5.3.sh" 2>/dev/null
"$IDF_PATH/tools/idf.py" fullclean
"$IDF_PATH/tools/idf.py" build
"$IDF_PATH/tools/idf.py" -p "$ESP32_PORT" flash
"$IDF_PATH/tools/idf.py" -p "$ESP32_PORT" monitor
SCRIPT
