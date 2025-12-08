# Shades Controller (Seeed XIAO ESP32C6 + Matter)

Roller shades controller for **Seeed Studio XIAO ESP32C6**. Drives a 28BYJ‑48 stepper via ULN2003, provides WiFiManager captive portal setup, calibration, web UI with QR-code pairing, and exposes shade position to **Matter** (as a dimmable light endpoint).

## Features

- **Matter endpoint** (exposed as a dimmable light slider; 0% = closed, 100% = open)
- **WiFiManager captive portal** for easy WiFi setup (AP: `RollerShades`, 180s timeout)
- **Web UI with Matter QR-code** for instant pairing via Home app or Google Home
- Physical buttons: UP / DOWN; MAIN actions via simultaneous UP+DOWN
- **Onboard LED** (GPIO15): blinks in CALIBRATE, steady in NORMAL
- Minimal persisted state (`/config.json`): currentStep, maxSteps, targetPositionValue, raw calibration points
- Continuous calibration jogging (constant speed; acceleration disabled)
- Web UI: movement control, presets (30% / 50% / 70%), calibration start/stop/save, safe reboot, factory reset
- Safe reboot: saves current position, sets target to it, HTML page with auto redirect, then restart

## Hardware

- **MCU:** Seeed Studio XIAO ESP32C6
- **Stepper:** 28BYJ‑48 (12V variant preferred for reliability and torque)
- **Driver:** ULN2003 board
- **Buttons:** 2 × tactile (wired to ground, `INPUT_PULLUP`, pressed = LOW)
- **LED:** Onboard digital LED on **GPIO15** (changed from GPIO9 due to SPI Flash conflict)
- **Power:** Stable supply matching motor voltage; ensure adequate current for both logic and motor

### Pin wiring

AccelStepper HALF4WIRE, coil order MOTOR_IN1, MOTOR_IN3, MOTOR_IN2, MOTOR_IN4:

- MOTOR_IN1 → D1 (GPIO1)
- MOTOR_IN2 → D2 (GPIO2)
- MOTOR_IN3 → D3 (GPIO21 / SPI SS)
- MOTOR_IN4 → D4 (GPIO22 / I2C SDA)
- BUTTON_UP_PIN → D8 (GPIO19 / SPI SCK)
- BUTTON_DOWN_PIN → D9 (GPIO20 / SPI MISO)
- LED_PIN → GPIO15 (onboard LED, digital output)

**XIAO ESP32C6 pin map:**
D0=GPIO0, D1=GPIO1, D2=GPIO2, D3=GPIO21, D4=GPIO22, D5=GPIO23, D6=GPIO16 (TX),
D7=GPIO17 (RX), D8=GPIO19 (SCK), D9=GPIO20 (MISO), D10=GPIO18 (MOSI), LED=GPIO15

**⚠️ Important:** GPIO9-15 are reserved for SPI Flash on ESP32C6 and cannot be used for peripherals like NeoPixel. The onboard LED is connected to GPIO15 and works as a simple digital output.

**Motor direction:** To reverse motor direction, flip the wiring order of the four ULN2003 inputs (e.g., IN1→D4, IN2→D3, IN3→D2, IN4→D1). No code changes required.

**Custom pins:** Adjust pins in `pins.h` if your wiring differs.

## First Setup

1. **Power on** the device
2. **Connect to WiFi:**
   - Device starts captive portal AP: **RollerShades** (no password, 180s timeout)
   - Connect to it, enter your WiFi credentials in the captive portal
   - Device saves credentials and connects to your network
3. **Access Web UI:**
   - Check your router for device IP address (e.g. `http://192.168.x.x`)
   - Web interface shows Matter QR-code and pairing code
4. **Pair with Matter** (optional):
   - iOS Home app: Add Accessory → Scan QR-code from web page
   - Or enter manual code: `34970112332`
5. **Calibrate** via web UI or buttons (see Calibration section)

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

- Factory reset (MAIN ~10s): clears Wi‑Fi creds, Matter fabric, and SPIFFS config file; restarts.
- Safe Reboot (web `/reboot`): stops motion, sets target=current position, saves config, serves HTML "Rebooting" page (auto redirect ~15s), restarts.

## Matter Endpoint

- The current Arduino Matter package does not expose a Window Covering endpoint, so the shades are mapped to a **Dimmable Light** endpoint where brightness == position (0% closed, 100% open). On/Off maps to closed/open.
- Commission with the manual pairing code `34970112332` or the QR URL printed on Serial (same as the Espressif examples).
- ESP32C6 uses BLE for commissioning; if you clear Matter (long press MAIN ~10s), the device decommissions and is ready for re‑pairing on next boot.

## Web UI

**Access:** `http://<device-ip>` (check your router for IP address)

**Main features:**

- **Matter Pairing section:** QR-code for instant pairing + manual code `34970112332`
- **Control:** Up/Down/Stop buttons, presets (30%/50%/70%)
- **Calibration:** Start/Exit, Save Top/Bottom positions
- **System:** Safe Reboot, Factory Reset

**Status polling:** JavaScript fetches `/status` every 200ms. Stop button blinks while moving.

**API Endpoints (POST unless marked GET):**

- `/cal/start`, `/cal/stop` - enter/exit calibration mode
- `/cal/up/start`, `/cal/down/start` - toggle continuous jog in CALIBRATE
- `/preset/30`, `/preset/50`, `/preset/70` - move to position
- `/cal/hold/stop` - stop jog or motion
- `/cal/saveTop`, `/cal/saveBottom` - save calibration points
- `/reboot` - safe reboot (saves state first)
- `/factory` - factory reset (clears WiFi + Matter + config)
- `/status` (GET) - JSON: `{ currentStep, maxSteps, mode, position, msg, moving }`

**Storage:** SPIFFS writes happen only on movement completion, calibration success, or reboot.

## 3D Files

Smart roller shades printable parts and enclosure:

- <https://www.printables.com/model/1505710-smart-roller-shades-esp8266-homekit>

## Software & Build

**Required:**

- **Board:** `esp32:esp32:XIAO_ESP32C6`
- **Partition scheme:** `huge_app` (3MB APP) - required for Matter builds
- **Libraries:** Matter, ArduinoJson v7, AccelStepper, EasyButton, WebServer, WiFiManager, LittleFS
- **Serial:** 115200 baud

### Compilation

```bash
# Build to ./build folder
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6:PartitionScheme=huge_app \
  --build-path ./build .
```

### Upload via USB

```bash
# Upload via USB (find port with: ls /dev/cu.*)
arduino-cli upload --fqbn esp32:esp32:XIAO_ESP32C6:PartitionScheme=huge_app \
  -p /dev/cu.usbmodem2101 \
  --input-dir ./build .
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

## Matter Integration

**Pairing:**

- Open web UI at device IP address
- Scan QR-code in Matter Pairing section with Home app (iOS) or Google Home
- Or enter manual code: **34970112332**

**Endpoint type:** Dimmable Light (native Window Covering endpoint not yet available in Arduino Matter)

- Brightness slider = shade position (0% closed, 100% open)
- On/Off switch = closed/open state

**Commissioning:** Uses BLE on ESP32C6 for initial pairing, then WiFi for control

**Factory reset:** Hold both buttons ~10s to clear Matter fabric and WiFi credentials

**Resources:**

- [Seeed XIAO ESP32C6 Matter Guide](https://wiki.seeedstudio.com/xiao_esp32_matter_env/)

## Troubleshooting

**Device not booting:**

- Check Serial Monitor (115200 baud) for boot messages
- Verify GPIO9-15 are not used for peripherals (reserved for SPI Flash on ESP32C6)

**WiFi not connecting:**

- Factory reset: hold both buttons ~10s
- Connect to "RollerShades" AP and reconfigure WiFi

**Matter pairing fails:**

- Verify device is not already commissioned (factory reset if needed)
- Use QR-code from web UI (auto-generated by Matter library)
- Check Serial Monitor for pairing status

## Inspiration

- Original design: [Thingiverse thing:2392856](https://www.thingiverse.com/thing:2392856/)
- 3D printable parts: [Printables model 1505710](https://www.printables.com/model/1505710-smart-roller-shades-esp8266-homekit)

## License

Non-commercial use (as-is).
