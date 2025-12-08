#include "LedControl.h"
#include "pins.h"
#include <Arduino.h>

#define LED_PWM_FREQ 5000
#define LED_PWM_RESOLUTION 8

namespace Led
{
  static uint8_t currentBrightness = 0;

  void begin()
  {
    // Setup LEDC PWM for LED (new Arduino ESP32 API)
    ledcAttach(LED_PIN, LED_PWM_FREQ, LED_PWM_RESOLUTION);
    off();
  }

  void off()
  {
    ledcWrite(LED_PIN, 255); // Inverted: HIGH = OFF
    currentBrightness = 0;
  }

  void setOn(uint8_t brightness)
  {
    ledcWrite(LED_PIN, (brightness > 0) ? 0 : 255); // Inverted: LOW = ON
    currentBrightness = (brightness > 0) ? 255 : 0;
  }

  void setBrightness(uint8_t brightness)
  {
    ledcWrite(LED_PIN, 255 - brightness); // Inverted: invert duty cycle
    currentBrightness = brightness;
  }

  void toggle()
  {
    if (currentBrightness > 0)
      off();
    else
      setOn(255);
  }

  bool isOn()
  {
    return currentBrightness > 0;
  }
}
