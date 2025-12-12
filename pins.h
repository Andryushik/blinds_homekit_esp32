// Seeed Studio XIAO ESP32C6 pin assignments (official Arduino variant)
#include <Arduino.h>

// ESP32C6 XIAO has built-in LED on GPIO15
#define LED_PIN 15

// Buttons use INPUT_PULLUP, pressed == LOW.
#define BUTTON_UP_PIN 19   // XIAO D8 (GPIO19 / SPI SCK)
#define BUTTON_DOWN_PIN 20 // XIAO D9 (GPIO20 / SPI MISO)

// Motor ULN2003 driver coil pins (AccelStepper 4-wire HALF4WIRE)
// Wiring on XIAO header: choose sequential pins for cleaner routing
#define MOTOR_IN1 1  // D1 (GPIO1)
#define MOTOR_IN2 2  // D2 (GPIO2)
#define MOTOR_IN3 21 // D3 (GPIO21 / SPI SS)
#define MOTOR_IN4 22 // D4 (GPIO22 / I2C SDA)
