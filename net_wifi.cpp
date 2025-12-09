#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "net_wifi.h"
#include "Globals.h"
#include "LedControl.h"

void wifiConnect()
{
  WiFiManager wifiManager;

  wifiManager.setAPCallback([](WiFiManager *myWiFiManager)
                            {
    DPRINTLN("Config portal started");
    DPRINTF("Connect to 'RollerShades' WiFi or go to: http://%s\n", WiFi.softAPIP().toString().c_str());
    // Turn LED ON bright during captive portal
    Led::setBrightness(255); });

  wifiManager.setSaveConfigCallback([]()
                                    { DPRINTLN("WiFi config saved"); });

  wifiManager.setConfigPortalTimeout(180);

  WiFi.setHostname("RollerShades");
  WiFi.setSleep(false);

  if (!wifiManager.autoConnect("RollerShades"))
  {
    DPRINTLN("WiFi timeout, restarting...");
    delay(3000);
    ESP.restart();
  }

  DPRINTF("WiFi connected: %s\n", WiFi.localIP().toString().c_str());
}

void wifiLoop()
{
  // Empty - no mDNS to maintain
}

void wifiReset()
{
  WiFiManager wifiManager;
  wifiManager.resetSettings();
}
