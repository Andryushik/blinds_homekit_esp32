#include "matter_bridge.h"
#include "Globals.h"

extern int targetPercent;

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include <Matter.h>
#include <MatterEndpoints/MatterDimmableLight.h>

namespace
{
  constexpr uint32_t REPORT_INTERVAL_MS = 600; // throttle updates while moving

  int clampPercent(int p)
  {
    if (p < 0)
      return 0;
    if (p > 100)
      return 100;
    return p;
  }

  uint8_t percentToLevel(int percent)
  {
    // Map 0-100% -> 0-255 with rounding
    long scaled = (long)percent * 255L + 50L;
    return (uint8_t)(scaled / 100L);
  }

  int levelToPercent(uint8_t level)
  {
    long scaled = (long)level * 100L + 127L;
    return (int)(scaled / 255L);
  }
}

namespace MatterBridge
{
  static MatterDimmableLight shadeEndpoint;
  static bool started = false;
  static int lastReportedPercent = -1;
  static uint32_t lastReportMs = 0;

  static void publishPercent(int percent, bool force)
  {
    if (!started)
      return;
    int p = clampPercent(percent);
    if (!force && p == lastReportedPercent)
      return;

    uint8_t level = percentToLevel(p);
    bool on = (p > 0);

    // Update attributes (maps percent to brightness)
    shadeEndpoint.setBrightness(level);
    shadeEndpoint.setOnOff(on);

    lastReportedPercent = p;
  }

  static bool onMatterChange(bool on, uint8_t brightness)
  {
    // Map Matter dimmer change back to targetPercent; off maps to 0%
    int percent = on ? levelToPercent(brightness) : 0;
    targetPercent = clampPercent(percent);
    return true;
  }

  void begin(int initialPercent)
  {
    int p = clampPercent(initialPercent);
    uint8_t level = percentToLevel(p);

    if (!shadeEndpoint.begin(p > 0, level))
    {
      DPRINTLN("Matter: failed to init dimmable endpoint");
      return;
    }

    shadeEndpoint.onChange(onMatterChange);

    Matter.begin();
    started = true;

    if (Matter.isDeviceCommissioned())
    {
      // Keep controller/UI in sync on reboot
      shadeEndpoint.updateAccessory();
    }
    publishPercent(p, true);
    lastReportMs = millis();
    DPRINTLN("Matter: endpoint ready");
  }

  void loop(int currentPercent, bool moving)
  {
    if (!started)
      return;

    uint32_t now = millis();
    bool percentChanged = (currentPercent != lastReportedPercent);

    // Throttle while moving to reduce attribute spam
    if (percentChanged && (!moving || (now - lastReportMs) >= REPORT_INTERVAL_MS))
    {
      publishPercent(currentPercent, false);
      lastReportMs = now;
    }
  }

  void factoryReset()
  {
    if (started)
    {
      Matter.decommission();
    }
  }

  bool isEnabled() { return started; }
}
#else
namespace MatterBridge
{
  void begin(int) {}
  void loop(int, bool) {}
  void factoryReset() {}
  bool isEnabled() { return false; }
}
#endif
