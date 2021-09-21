#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "Logger.h"
#include "HttpServer.h"
#include "EEPROMConfiguration.h"
#include "WS281xDriver.h"

#include "NetworkManager.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

NetworkManager::NetworkManager() : _tickNTPUpdate(0), _brightnessOnLastPublish(-1), _lastPublishMillis(0)
{}

NetworkManager::~NetworkManager() 
{}

void NetworkManager::setup()
{
    /* Initialize Wifi */
    _wifiManager.setDebugOutput(true);
    // wifiManager.resetSettings();

    // WiFiManagerParameter
    //WiFiManagerParameter custom_mqtt_hostname("hostname", "hostname", Configuration._hostname.c_str(), 60);
    //wifiManager.addParameter(&custom_mqtt_hostname);

    Log.println("Try to connect to WiFi...");
    _wifiManager.setConfigPortalTimeout(300); // Set Timeout for portal configuration to xx seconds
    if (not _wifiManager.autoConnect(Configuration.hostname().c_str()))
    {
        Log.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
    }

    Log.println(String("Connected to ") + WiFi.SSID());
    Log.println(String("IP address: ") + WiFi.localIP().toString());

    /* Initialize HTTP Server */
    HTTPServer.setup();

    /* Initialize OTA Server */
    Log.println("Arduino OTA activated");

    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(Configuration.hostname().c_str());

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
    MDNS.begin(Configuration.hostname().c_str());
    MDNS.addService("http", "tcp", 80);
/*
    _mqttClient.setServer(IPAddress().fromString(Configuration.mqttServerIp()), Configuration.mqttServerPort());
    _mqttClient.setClient(_wifiClient);
    _mqttClient.setCallback([this] (char* topic, uint8_t* payload, unsigned int length) { this->mqttMessageReceived(topic, payload, length); });
    */
    //we build base topic from configuration
    _baseTopic = Configuration.hostname();
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
/*
    _mqttClient.loop();
    if (not _mqttClient.connected())
        reconnectMqtt();*/
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

   
void NetworkManager::reconnectMqtt() 
{
  // Loop until we're reconnected
  if (not _mqttClient.connected()) 
  {
    _mqttConnectionAttempts++;
    if (_mqttConnectionAttempts < 10)
    {
      Log.print("Attempting MQTT connection (" + String(_mqttConnectionAttempts) + ") ...");
      // Create a random client ID
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);

      Log.println("_wifiClient.connected() : " + String(_wifiClient.connected()));
      Log.println("local ip");
      Log.println(WiFi.localIP().toString());

      // Attempt to connect
      Log.println("Connecting to " + Configuration.mqttServerIp() + " with port " + Configuration.mqttServerPort());
      Log.println("With clientId : " + clientId + ", login : " + Configuration.mqttLogin() + " password : " + Configuration.mqttPassword());
      _mqttClient.connect(clientId.c_str(), Configuration.mqttLogin().c_str(), Configuration.mqttPassword().c_str());
      Log.println("Connection state: " + String(_mqttClient.state()));
      if (_mqttClient.state() == 0) 
      {
        Log.println("connected");
        // Once connected, publish an announcement...
        publishMqttState();
        // ... and resubscribe
        _mqttClient.unsubscribe((_baseTopic + String("/cmnd")).c_str());
        _mqttClient.subscribe((_baseTopic + String("/cmnd")).c_str());
      } 
      else 
      {
        Log.println("failed");
        Log.println("Try again in 2 seconds");
        // Wait 2 seconds before retrying
        delay(2000);
      }
    }
  }
}

void NetworkManager::publishMqttState()
{
    if ((LedDriver.globalBrightness() != _brightnessOnLastPublish) || (millis() - _lastPublishMillis > 10*1000))
    {
        DynamicJsonDocument conf(4096);

        conf["brightness"] = LedDriver.globalBrightness();
        conf["color_mode"] = String("rgb");
        conf["state"] = String("ON");

        String output;  
        serializeJson(conf, output);

        //_mqttClient.publish((_baseTopic + String("/state")).c_str(), output.c_str());
        _brightnessOnLastPublish = LedDriver.globalBrightness();
        _lastPublishMillis = millis();
    }
}

void NetworkManager::mqttMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
  /*String data;

  Log.print("Message arrived [");
  Log.print(topic);
  Log.print("] ");

  for (unsigned int i = 0 ; i < length ; i++) {
    Log.print(String(payload[i]));
    data += (char)payload[i];
  }
  Log.println();

  String topicStr(topic);
  topicStr.remove(0, topicStr.lastIndexOf('/')+1);

  if (topicStr == String("cmnd")) 
  {
    /*int status = data.toInt();
    digitalWrite(RELAY_PIN, status);
    Log.println(String("set relay status to ") + String(status));
    publish(String("/relay"), String(status));
  }*/
}


#if !defined(NO_GLOBAL_INSTANCES) 
NetworkManager Network;
#endif
