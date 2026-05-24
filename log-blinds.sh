#!/usr/bin/env bash
# Telnet debug console capturer with auto-reconnect.
# Writes timestamped + tagged lines to blinds.log next to this script.
#
# Usage:
#   ./log-blinds.sh                 # default: left (192.168.2.220)
#   ./log-blinds.sh left
#   ./log-blinds.sh right           # 192.168.2.221
#   ./log-blinds.sh <host-or-ip>    # arbitrary; tag = host
#
# To follow both devices in parallel (lines are atomic per-write so they
# won't corrupt; they will just interleave):
#   nohup ./log-blinds.sh left  > /dev/null 2>&1 &
#   nohup ./log-blinds.sh right > /dev/null 2>&1 &
#   disown
#   tail -f blinds.log

PORT=23
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LOG="$SCRIPT_DIR/blinds.log"

case "${1:-left}" in
  left)  HOST="192.168.2.220"; TAG="left"  ;;
  right) HOST="192.168.2.221"; TAG="right" ;;
  *)     HOST="$1";             TAG="$1"    ;;
esac

while true; do
  printf '[%s][%s] === connecting to %s:%s ===\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$TAG" "$HOST" "$PORT" >> "$LOG"
  # -w 15: drop the connection if no data for 15 s. Device sends a heartbeat
  # every 5 s, so 15 s of silence = half-open or dead peer → force reconnect.
  nc -w 15 "$HOST" "$PORT" 2>&1 | while IFS= read -r line; do
    printf '[%s][%s] %s\n' "$(date '+%H:%M:%S')" "$TAG" "$line" >> "$LOG"
  done
  printf '[%s][%s] === disconnected, reconnecting in 3s ===\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$TAG" >> "$LOG"
  sleep 3
done
