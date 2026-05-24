#!/usr/bin/env bash
# Compile WITHOUT SHADES_DEBUG and OTA-flash the device(s).
# No DPRINTs, no telnet console, no heartbeats — production firmware.
#
# Usage:
#   ./flash-release.sh                    # default: left (192.168.2.220)
#   ./flash-release.sh left
#   ./flash-release.sh right              # 192.168.2.221
#   ./flash-release.sh both               # compile once, flash both sequentially
#   ./flash-release.sh Blinds-174C.local  # arbitrary host (mDNS or IP)

set -e

LEFT="192.168.2.220"
RIGHT="192.168.2.221"
FQBN="esp32:esp32:XIAO_ESP32C6:PartitionScheme=min_spiffs"
OTA_PASSWORD="28142814"
BUILD_DIR="./build/release"

cd "$(dirname "$0")"

case "${1:-left}" in
  left)  TARGETS=("$LEFT") ;;
  right) TARGETS=("$RIGHT") ;;
  both)  TARGETS=("$LEFT" "$RIGHT") ;;
  *)     TARGETS=("$1") ;;
esac

echo "==> compile (release)"
arduino-cli compile --fqbn "$FQBN" \
  --output-dir "$BUILD_DIR" \
  .

for target in "${TARGETS[@]}"; do
  echo "==> OTA upload to $target"
  arduino-cli upload --fqbn "$FQBN" \
    --input-dir "$BUILD_DIR" \
    --port "$target" -F password="$OTA_PASSWORD" \
    .
done
