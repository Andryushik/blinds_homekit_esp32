#!/usr/bin/env bash
# Background telnet logger for the blinds debug console (port 23).
# Auto-reconnects if the device reboots or the session drops.
# Output goes to blinds.log next to this script.
#
# Usage:
#   ./log-blinds.sh [host] [port]                # tail in foreground
#   nohup ./log-blinds.sh > /dev/null 2>&1 &     # detached background
#   pkill -f log-blinds.sh                       # stop
#
# Tail live:
#   tail -f blinds.log

HOST="${1:-192.168.2.220}"
PORT="${2:-23}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LOG="$SCRIPT_DIR/blinds.log"

while true; do
  printf '[%s] === connecting to %s:%s ===\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$HOST" "$PORT" >> "$LOG"
  # -w 15: drop the connection if no data for 15 s. Device sends a heartbeat
  # every 5 s, so 15 s of silence = half-open or dead peer → force reconnect.
  nc -w 15 "$HOST" "$PORT" 2>&1 | while IFS= read -r line; do
    printf '[%s] %s\n' "$(date '+%H:%M:%S')" "$line" >> "$LOG"
  done
  printf '[%s] === disconnected, reconnecting in 3s ===\n' "$(date '+%Y-%m-%d %H:%M:%S')" >> "$LOG"
  sleep 3
done
