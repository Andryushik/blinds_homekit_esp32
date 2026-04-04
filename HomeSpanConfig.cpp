#include "HomeSpanConfig.h"
#include "HomeSpan.h"
#include "HomeKitShade.h"

// Device identity built from MAC in main .ino
extern char deviceHostname[];
extern char deviceApSSID[];
extern char deviceSerial[];

void homeSpanSetup()
{
  // Enable OTA updates (must be called before begin)
  homeSpan.enableOTA("28142814");

  // Initialize HomeSpan
  homeSpan.begin(Category::WindowCoverings, deviceHostname);

  // Enable Auto-AP mode without password for easy WiFi configuration
  homeSpan.setApSSID(deviceApSSID);
  homeSpan.setApPassword("");
  homeSpan.enableAutoStartAP();

  homeSpan.setPairingCode("28142814");

  // Give HomeSpan internal tasks time to initialize
  delay(100);

  // Create HomeKit accessory structure
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Characteristic::Name(deviceHostname);
  new Characteristic::Manufacturer("DIY");
  new Characteristic::Model("ESP32C6-Blinds");
  new Characteristic::SerialNumber(deviceSerial);
  new Blinds();
}
