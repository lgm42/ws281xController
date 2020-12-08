#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

#include "Logger.h"
#include "HttpServer.h"
#include "JsonConfiguration.h"

#include "NetworkManager.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

NetworkManager::NetworkManager() : _tickNTPUpdate(0) 
{}

NetworkManager::~NetworkManager() 
{}

void NetworkManager::setup()
{
    /* Initialize Wifi */
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);
    // wifiManager.resetSettings();

    // WiFiManagerParameter
    //WiFiManagerParameter custom_mqtt_hostname("hostname", "hostname", Configuration._hostname.c_str(), 60);
    //wifiManager.addParameter(&custom_mqtt_hostname);

    Log.println("Try to connect to WiFi...");
    wifiManager.setConfigPortalTimeout(300); // Set Timeout for portal configuration to xx seconds
    if (!wifiManager.autoConnect(Configuration._hostname.c_str()))
    {
        Log.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
    }

    Log.println(String("Connected to ") + WiFi.SSID());
    Log.println(String("IP address: ") + WiFi.localIP().toString());

    /* Get configuration from WifiManager */
    //Configuration._hostname = custom_mqtt_hostname.getValue();

    /* Initialize HTTP Server */
    HTTPServer.setup();

    /* Initialize OTA Server */
    Log.println("Arduino OTA activated");

    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(Configuration._hostname.c_str());

    ArduinoOTA.onStart([&]() {
        Log.println("Arduino OTA: Start updating");
    });

    ArduinoOTA.onEnd([]() {
        Log.println("Arduino OTA: End");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Log.print("Arduino OTA Progress: ");
        Log.print(String(progress / (total / 100)));
        Log.println(" %");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Log.print("Arduino OTA Error : ");
        Log.print(String(error));
        if (error == OTA_AUTH_ERROR)
            Log.println("Arduino OTA: Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Log.println("Arduino OTA: Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Log.println("Arduino OTA: Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Log.println("Arduino OTA: Receive Failed");
        else if (error == OTA_END_ERROR)
            Log.println("Arduino OTA: End Failed");
    });

    ArduinoOTA.begin();

    /* Initialize MDNS */
    MDNS.begin(Configuration._hostname.c_str());
    MDNS.addService("http", "tcp", 80);
}

void NetworkManager::handle()
{
    HTTPServer.handle();
    ArduinoOTA.handle();
    MDNS.update();

    unsigned long currentMillis = millis();
    if ((currentMillis - _tickNTPUpdate) >= (unsigned long)(NTP_UPDATE_INTERVAL_MSEC))
    {
        updateNTP();
        _tickNTPUpdate = currentMillis;
    }
}

void NetworkManager::updateNTP()
{
  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  delay(500);
  while (!time(nullptr))
  {
    Log.print("#");
    delay(1000);
  }
  Log.println("Update NTP");
}

#if !defined(NO_GLOBAL_INSTANCES) 
NetworkManager Network;
#endif
