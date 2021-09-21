#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Ethernet.h>
#include <PubSubClient.h>

class NetworkManager 
{
  public:
	NetworkManager();
  	virtual ~NetworkManager();

	void setup();
	void handle();

  void publishMqttState();

  private:
    void updateNTP();
    void reconnectMqtt();
    void mqttMessageReceived(char* topic, uint8_t* payload, unsigned int length);
    
    unsigned long _tickNTPUpdate;
    WiFiManager _wifiManager;
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    String _baseTopic;
    int _brightnessOnLastPublish;
    long _lastPublishMillis;
    int _mqttConnectionAttempts;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern NetworkManager Network;
#endif
