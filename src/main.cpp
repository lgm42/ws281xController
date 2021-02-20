#include <Arduino.h>
#include <WS2812FX.h>
#include <ArduinoOTA.h>

#include "Logger.h"
#include "EEPROMConfiguration.h"
#include "NetworkManager.h"
#include "WS281xDriver.h"
#include "BtnManager.h"

void setup() {
  delay(100);

  /* Initialize Logger */
  Log.setup();
  Log.println();
  Log.println(String(F("=== WS281X Controller ===")));
  Log.println(String(F("  Version: ")) + F(VERSION));
  Log.println(String(F("  Build: ")) + F(__DATE__) + " " + F(__TIME__));

  Configuration.setup();
  //Configuration.restoreDefault();

  Network.setup();
  LedDriver.setup();
  ButtonManager.setup();

  digitalWrite(LED_BUILTIN, 0);
}

void loop() {
  Network.handle();
  LedDriver.handle();
  Log.handle();
  ButtonManager.handle();
}
