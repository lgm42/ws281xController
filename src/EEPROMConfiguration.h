#pragma once

#include "settings.h"

class EEPROMConfiguration 
{
  public:
  	EEPROMConfiguration();
  	virtual ~EEPROMConfiguration();
  
  	void setup();

    bool readConfig();
  	void saveConfig();
  
  	void restoreDefault();
    
    String toJson();
  
	String hostname();
	String mqttIpServer();
	uint16_t mqttPortServer();
	String mqttLogin();
	String mqttPassword();
	uint32_t colorLedOn();
	uint16_t numLeds();
	uint16_t neoPixelType();

  private:
    
  typedef struct 
	{
		uint32_t eepromValidity;
		char hostname[30];
		char mqttIpServer[30];
		uint16_t mqttPortServer;
		char mqttLogin[30];
		char mqttPassword[30];
		uint32_t colorLedOn;
		uint16_t numLeds;
		uint16_t neoPixelType;
	} rawData;

	rawData _data;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern EEPROMConfiguration Configuration;
#endif
