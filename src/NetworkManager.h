#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

class NetworkManager 
{
  public:
	NetworkManager();
  	virtual ~NetworkManager();

	void setup();
	void handle();

  private:
    void updateNTP();
    
    unsigned long _tickNTPUpdate;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern NetworkManager Network;
#endif
