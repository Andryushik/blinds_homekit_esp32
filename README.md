# Shades Controller (Seeed XIAO ESP32C6, Matter-ready)

Roller shades controller for Seeed Studio XIAO ESP32C6. Drives a 28BYJ‑48 stepper via ULN2003, provides calibration, web UI, OTA updates, and exposes shade position to Matter (as a dimmable light; HomeKit code removed).

## Features

- Matter endpoint (exposed as a dimmable light slider; 0% = closed, 100% = open). HomeKit code removed.
- Physical buttons: UP / DOWN; MAIN actions via simultaneous UP+DOWN.
- LED feedback: blinks in CALIBRATE, steady (LOW, active‑low) in NORMAL.
- Minimal persisted state (`/config.json`): currentStep, maxSteps, targetPositionValue, raw calibration points.
- Wi‑Fi STA with stored credentials; fallback open AP `RollerShades` if STA fails. Matter commissioning (BLE on ESP32C6) can provision Wi‑Fi.
- OTA updates via ArduinoOTA (hostname `roller_shades`).
- Continuous calibration jogging (constant speed; acceleration disabled).
- Web UI: movement control, presets (30% / 50% / 70%), calibration start/stop/save, safe reboot, factory reset.
- Safe reboot: saves current position, sets target to it, HTML page with auto redirect, then restart.

## Hardware

- MCU: Seeed Studio XIAO ESP32C6
- Stepper: 28BYJ‑48 — 12 V variant preferred for reliability and torque; ensure the ULN2003 board and power supply match the motor voltage.
- Driver: ULN2003 board.
- Buttons: 2 × tactile (wired to ground, `INPUT_PULLUP`, pressed = LOW).
- LED: onboard single addressable RGB (WS2812/NeoPixel) on GPIO9 (not the D9 header pin).
- Power: stable supply matching motor voltage; ensure adequate current for both logic and motor.

Pin wiring (AccelStepper HALF4WIRE, coil order MOTOR_IN1, MOTOR_IN3, MOTOR_IN2, MOTOR_IN4 as used in code):

- MOTOR_IN1 → D1 (GPIO1)
- MOTOR_IN2 → D2 (GPIO2)
- MOTOR_IN3 → D3 (GPIO21 / SPI SS)
- MOTOR_IN4 → D4 (GPIO22 / I2C SDA)
- BUTTON_UP_PIN → D8 (GPIO19 / SPI SCK)
- BUTTON_DOWN_PIN → D9 (GPIO20 / SPI MISO)
- LED pixel DIN → GPIO9 (built-in NeoPixel)

Seeed Studio XIAO ESP32C6 pin map (per official Arduino variant `pins_arduino.h`):
D0=GPIO0, D1=GPIO1, D2=GPIO2, D3=GPIO21, D4=GPIO22, D5=GPIO23, D6=GPIO16 (TX),
D7=GPIO17 (RX), D8=GPIO19 (SCK), D9=GPIO20 (MISO), D10=GPIO18 (MOSI), LED=GPIO15,
I2C SDA=GPIO22, I2C SCL=GPIO23, SPI SS=GPIO21.

On-board addressable RGB LED (WS2812-compatible) is wired to GPIO9. Use NeoPixel/FastLED
libraries; it is not controllable via `digitalWrite`/`analogWrite`.

To reverse motor direction just flip the wiring order of the four ULN2003 inputs. For example: IN1→D1, IN2→D5, IN3→D6, IN4→D7 (no code changes are required when you flip the wiring).

Note: Adjust pins in `pins.h` if your wiring differs.

## First Setup

1. Power the device. If Wi‑Fi creds are stored it connects as STA; otherwise it starts an open AP `RollerShades` and also awaits Matter commissioning (BLE on ESP32C6).
2. Commission via Matter (preferred): add a new accessory in your controller, use the manual code `34970112332` or the QR URL printed on Serial, and complete pairing to push Wi‑Fi credentials.
3. Calibrate via web UI or buttons.

## Calibration

Goal: learn TOP and BOTTOM raw steps to derive travel (`maxSteps`). Uses constant speed (`CAL_SPEED`) with acceleration disabled for smoothness.

Flow:

1. Automatic entry if no calibration present; or hold BOTH ~5s.
2. Jog: press UP (or DOWN) to start continuous motion; press same again to stop (toggle behavior).
3. Save TOP: short BOTH press. Then jog to bottom.
4. Save BOTTOM: short BOTH press again → success if travel ≥ `MIN_TRAVEL = 4096` steps.
5. On success: rebase (TOP=0, BOTTOM=`maxSteps`), persist, exit to NORMAL.

Tips:

- LED blinks while in calibration.
- Stop motion before saving (for precision).
- If "Travel too small" message appears, increase physical travel and retry.

## Manual Control (NORMAL mode)

- UP: set target to 100% (fully open).
- DOWN: set target to 0% (closed).
- MAIN (UP+DOWN): short press STOP (target := current position, position state STOPPED); ~5s enter CALIBRATE; ~10s factory reset.

## Configuration Persistence

Saved keys: `currentStep`, `maxSteps`, `targetPositionValue`, `rawUpStep`, `rawDownStep`.

Save triggers:

- Movement completes in NORMAL mode.
- Successful calibration.
- Just before safe reboot.

## Reset & Safe Reboot

- Factory reset (MAIN ~10s): clears Wi‑Fi creds, Matter fabric, and LittleFS config file; restarts.
- Safe Reboot (web `/reboot`): stops motion, sets target=current position, saves config, serves HTML "Rebooting" page (auto redirect ~15s), restarts.

## Matter Endpoint

- The current Arduino Matter package does not expose a Window Covering endpoint, so the shades are mapped to a **Dimmable Light** endpoint where brightness == position (0% closed, 100% open). On/Off maps to closed/open.
- Commission with the manual pairing code `34970112332` or the QR URL printed on Serial (same as the Espressif examples).
- ESP32C6 uses BLE for commissioning; if you clear Matter (long press MAIN ~10s), the device decommissions and is ready for re‑pairing on next boot.
- Once commissioned, Matter provides the Wi‑Fi credentials used by the web UI and OTA.

## Web UI

Root (`/`): Mode, current step, maxSteps, percent position, last message, buttons: Up / Down / Stop, Presets 30/50/70, Start/Exit Calibration, Save Top/Bottom, Safe Reboot, Factory Reset.

Status polling: JS fetches `/status` every 200 ms. Stop button blinks while position changes. Messages display movement or calibration feedback (errors highlighted).

Endpoints (POST unless marked GET):

- `/cal/start`, `/cal/stop`
- `/cal/up/start`, `/cal/down/start` (toggle continuous jog in CALIBRATE)
- `/preset/30`, `/preset/50`, `/preset/70` (move to 30% / 50% / 70%)
- `/cal/hold/stop` (stop jog OR STOP motion in NORMAL)
- `/cal/saveTop`, `/cal/saveBottom`
- `/reboot` (safe reboot)
- `/factory` (factory reset)
- `/status` (GET): JSON `{ currentStep, maxSteps, mode, position, msg }`

Writes to SPIFFS limited to essential moments (movement completion, calibration success, reboot).

You can reach the web UI either by entering the device IP in your browser (for example `http://192.168.x.y/`) or, once mDNS is active, via `http://roller_shades.local/` on the same LAN.

## 3D Files

Smart roller shades printable parts and enclosure:

- <https://www.printables.com/model/1505710-smart-roller-shades-esp8266-homekit>

## Software & Build

- Board: `esp32:esp32:XIAO_ESP32C6`
- Libraries: Matter (ESP32 Arduino), ArduinoJson v7, AccelStepper, EasyButton, WebServer, LittleFS,
  ArduinoOTA, Adafruit_NeoPixel (for the on-board WS2812).
- Serial: 115200 baud.
- Partition scheme: select `huge_app` (or another >2.8 MB app slot) for Matter builds.

Build & upload via Arduino CLI:

```zsh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 --build-property build.partitions=huge_app .
arduino-cli upload -p /dev/tty.usbmodemXXXX --fqbn esp32:esp32:XIAO_ESP32C6 --build-property build.partitions=huge_app .
```

OTA update (device hostname `roller_shades`):

```zsh
python3 $HOME/Library/Arduino15/packages/esp32/tools/espota/*/espota.py \
  -i roller_shades.local -p 3232 -f /path/to/firmware.bin
```

### Constants & Configuration

Timing constants (configurable in `Globals.h`):

- `BUTTON_IGNORE_WINDOW_MS = 10000` (10s button ignore after boot)
- `MAIN_LONG_PRESS_CAL_MS = 5000` (5s press for calibration mode)
- `MAIN_LONG_PRESS_RESET_MS = 10000` (10s press for factory reset)
- `PRESET_DEFER_WINDOW_MS = 150` (debounce for preset actions)
- `LED_BLINK_INTERVAL_MS = 400` (LED blink rate)

### Debugging

- Serial debug: 115200 baud (enable with `-DSHADES_DEBUG` flag).
- Build with debug:

```zsh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 --build-property build.partitions=huge_app \
  --build-property build.extra_flags="-DSHADES_DEBUG" .
```

## Matter Integration (ESP32C6)

- Seeed guide: https://wiki.seeedstudio.com/xiao_esp32_matter_env/
- Arduino Matter exposes the shades as a dimmable light (position slider) because a native window-covering endpoint is not yet available.
- Commission over BLE with the code `34970112332` (or the QR URL printed on Serial).
```

## Inspiration

- <https://www.thingiverse.com/thing:2392856/>

## License

Non‑commercial use (as‑is).
