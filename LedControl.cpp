#include "LedControl.h"
#include <Adafruit_NeoPixel.h>
#include "pins.h"

namespace Led
{
  // Single on-board pixel (GRB order, 800 kHz)
  static Adafruit_NeoPixel pixel(LED_PIXEL_COUNT, LED_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
  static uint8_t currentBrightness = 0;
  static const uint8_t BASE_R = 255;
  static const uint8_t BASE_G = 255;
  static const uint8_t BASE_B = 255;

  static void showColor(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
  {
    currentBrightness = brightness;
    pixel.setBrightness(brightness);
    if (brightness == 0)
    {
      pixel.clear();
    }
    else
    {
      pixel.setPixelColor(0, pixel.Color(r, g, b));
    }
    pixel.show();
  }

  void begin()
  {
    pixel.begin();
    pixel.clear();
    pixel.show();
    currentBrightness = 0;
  }

  void off()
  {
    showColor(0, 0, 0, 0);
  }

  void setOn(uint8_t brightness)
  {
    if (brightness == 0)
    {
      off();
      return;
    }
    showColor(BASE_R, BASE_G, BASE_B, brightness);
  }

  void setBrightness(uint8_t brightness)
  {
    setOn(brightness);
  }

  void toggle()
  {
    if (currentBrightness == 0)
      setOn();
    else
      off();
  }

  bool isOn()
  {
    return currentBrightness > 0;
  }
}
