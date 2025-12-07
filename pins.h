// Seeed Studio XIAO ESP32C6 pin assignments (official Arduino variant)
// Reference: https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/ and
// variant pins_arduino.h:
// D0=GPIO0, D1=GPIO1, D2=GPIO2, D3=GPIO21, D4=GPIO22, D5=GPIO23, D6=GPIO16,
// D7=GPIO17, D8=GPIO19, D9=GPIO20, D10=GPIO18; on-board RGB pixel DIN=GPIO9.
// Notes:
// - Buttons use INPUT_PULLUP, pressed == LOW.
// - ULN2003 driver for 28BYJ-48 stepper via AccelStepper HALF4WIRE.
// - On-board RGB LED is WS2812-compatible on GPIO9 (not the D9 header pin).
#include <Arduino.h>

// On-board addressable RGB LED (NeoPixel/WS2812) data line
#define LED_PIXEL_PIN 9
#define LED_PIXEL_COUNT 1
// Compatibility alias for existing code paths
#define LED_PIN LED_PIXEL_PIN

// Buttons
#define BUTTON_UP_PIN D8   // XIAO D8 (GPIO19 / SPI SCK)
#define BUTTON_DOWN_PIN D9 // XIAO D9 (GPIO20 / SPI MISO)

// Motor ULN2003 driver coil pins (AccelStepper 4-wire HALF4WIRE)
// Wiring on XIAO header: choose sequential pins for cleaner routing
#define MOTOR_IN1 D1 // D1 (GPIO1)
#define MOTOR_IN2 D2 // D2 (GPIO2)
#define MOTOR_IN3 D3 // D3 (GPIO21 / SPI SS)
#define MOTOR_IN4 D4 // D4 (GPIO22 / I2C SDA)
