#!/usr/bin/env bash
# Compile with SHADES_DEBUG and OTA-flash the device(s).
# DPRINTs + telnet console on port 23 + heartbeats are active in this build.
#
# Usage:
#   ./flash-debug.sh                    # default: left (192.168.2.220)
#   ./flash-debug.sh left
#   ./flash-debug.sh right              # 192.168.2.221
#   ./flash-debug.sh both               # compile once, flash both sequentially
#   ./flash-debug.sh Blinds-174C.local  # arbitrary host (mDNS or IP)

set -e

LEFT="192.168.2.220"
RIGHT="192.168.2.221"
FQBN="esp32:esp32:XIAO_ESP32C6:PartitionScheme=min_spiffs"
OTA_PASSWORD="28142814"
BUILD_DIR="./build/debug"

cd "$(dirname "$0")"

case "${1:-left}" in
  left)  TARGETS=("$LEFT") ;;
  right) TARGETS=("$RIGHT") ;;
  both)  TARGETS=("$LEFT" "$RIGHT") ;;
  *)     TARGETS=("$1") ;;
esac

echo "==> compile (debug)"
arduino-cli compile --fqbn "$FQBN" \
  --build-property "compiler.cpp.extra_flags=-DSHADES_DEBUG" \
  --output-dir "$BUILD_DIR" \
  .

for target in "${TARGETS[@]}"; do
  echo "==> OTA upload to $target"
  arduino-cli upload --fqbn "$FQBN" \
    --input-dir "$BUILD_DIR" \
    --protocol network \
    --port "$target" -F password="$OTA_PASSWORD" \
    .
done
