#include "Globals.h"

#if ENABLE_MATTER
#include "matter_bridge.h"

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
  static bool ignoreNextCallback = false; // Flag to prevent feedback loop

  static void publishPercent(int percent, bool force)
  {
    if (!started)
      return;
    int p = clampPercent(percent);
    if (!force && p == lastReportedPercent)
      return;

    uint8_t level = percentToLevel(p);
    bool on = (p > 0);

    // Prevent onChange callback from triggering during our own updates
    ignoreNextCallback = true;

    // Update attributes (maps percent to brightness)
    shadeEndpoint.setBrightness(level);
    shadeEndpoint.setOnOff(on);

    lastReportedPercent = p;
  }

  static bool onMatterChange(bool on, uint8_t brightness)
  {
    // Ignore callbacks triggered by our own publishPercent() calls
    if (ignoreNextCallback)
    {
      ignoreNextCallback = false;
      return true;
    }

    // Map Matter dimmer change back to targetPercent; off maps to 0%
    int percent = on ? levelToPercent(brightness) : 0;
    int newTarget = clampPercent(percent);

    Serial.printf("Matter: command received %d%% (current target was %d%%)\n", newTarget, targetPercent);
    targetPercent = newTarget;
    return true;
  }

  void begin(int initialPercent)
  {
    int p = clampPercent(initialPercent);
    uint8_t level = percentToLevel(p);

    if (!shadeEndpoint.begin(p > 0, level))
    {
      DPRINTLN("Matter: failed to init endpoint");
      return;
    }

    shadeEndpoint.onChange(onMatterChange);
    Matter.begin();
    started = true;

    if (Matter.isDeviceCommissioned())
    {
      Serial.println("Matter: устройство уже добавлено");
      shadeEndpoint.updateAccessory();
    }
    else
    {
      Serial.println("\n=== MATTER PAIRING ===");
      Serial.println("QR Code URL: " + Matter.getOnboardingQRCodeUrl());
      Serial.println("Manual code: " + Matter.getManualPairingCode());
      Serial.println("======================\n");
    }

    publishPercent(p, true);
    lastReportMs = millis();
    DPRINTLN("Matter: ready");
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

  String getQRCodeUrl()
  {
    if (!started || Matter.isDeviceCommissioned())
      return "";
    return Matter.getOnboardingQRCodeUrl();
  }

  String getPairingCode()
  {
    if (!started || Matter.isDeviceCommissioned())
      return "";
    return Matter.getManualPairingCode();
  }
}
#else
namespace MatterBridge
{
  void begin(int) {}
  void loop(int, bool) {}
  void factoryReset() {}
  bool isEnabled() { return false; }
  String getQRCodeUrl() { return ""; }
  String getPairingCode() { return ""; }
}
#endif
#else // ENABLE_MATTER
#include "matter_bridge.h"
namespace MatterBridge
{
  void begin(int) {}
  void loop(int, bool) {}
  void factoryReset() {}
  bool isEnabled() { return false; }
}
#endif
