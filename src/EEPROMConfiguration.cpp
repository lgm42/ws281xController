#include <EEPROM.h>

#include "Logger.h"
#include "EEPROMConfiguration.h"

#define START_ADDRESS 10
#define EXPECTED_EEPROM_VALIDITY  0xAB12CD89

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

EEPROMConfiguration::EEPROMConfiguration()
{
}

EEPROMConfiguration::~EEPROMConfiguration()
{
}

void EEPROMConfiguration::setup(void)
{
  EEPROM.begin(sizeof(rawData) + START_ADDRESS);
  Log.print ("sizeof(rawData):");
  Log.println (String(sizeof(rawData) + START_ADDRESS));

	if (!readConfig()) {
		Log.println("Invalid configuration values, restoring default values");
		restoreDefault();
	}

	Log.println(String("    hostname: ") + String(_data.hostname));
	Log.println(String("    mqttIpServer: ") + String(_data.mqttIpServer));
	Log.println(String("    mqttPortServer: ") + String(_data.mqttPortServer));
	Log.println(String("    mqttLogin: ") + String(_data.mqttLogin));
	Log.println(String("    mqttPassword: ") + String(_data.mqttPassword));
	Log.println(String("    colorLedOn: ") + String(_data.colorLedOn));
	Log.println(String("    numLeds: ") + String(_data.numLeds));
	Log.println(String("    neoPixelType: ") + String(_data.neoPixelType));
}

bool EEPROMConfiguration::readConfig()
{
  Log.println("Read Configuration file from EEPROM...");
  EEPROM.get(START_ADDRESS, _data);
  return _data.eepromValidity == EXPECTED_EEPROM_VALIDITY;
}

void EEPROMConfiguration::saveConfig()
{
  EEPROM.put (START_ADDRESS, _data);
  EEPROM.commit();
}

String EEPROMConfiguration::hostname()
{
    return String(_data.hostname);
}

String EEPROMConfiguration::mqttIpServer()
{
    return String(_data.mqttIpServer);
}

uint16_t EEPROMConfiguration::mqttPortServer()
{
    return _data.mqttPortServer;
}

String EEPROMConfiguration::mqttLogin()
{
    return String(_data.mqttLogin);
}

String EEPROMConfiguration::mqttPassword()
{
    return String(_data.mqttPassword);
}

uint32_t EEPROMConfiguration::colorLedOn()
{
    return _data.colorLedOn;
}

uint16_t EEPROMConfiguration::numLeds()
{
    return _data.numLeds;
}

uint16_t EEPROMConfiguration::neoPixelType()
{
    return _data.neoPixelType;
}

void EEPROMConfiguration::restoreDefault()
{  
  _data.eepromValidity = EXPECTED_EEPROM_VALIDITY;
  strcpy(_data.hostname, DEFAULT_HOSTNAME);
  strcpy(_data.mqttIpServer, DEFAULT_MQTTIPSERVER); 
  _data.mqttPortServer = DEFAULT_MQTTPORTSERVER; 
  strcpy(_data.mqttLogin, "");
  strcpy(_data.mqttPassword, "");
  _data.numLeds = DEFAULT_WS281X_LED_COUNT;
  _data.neoPixelType = DEFAULT_WS281X_NEO_PIXEL_TYPE;
  _data.colorLedOn = DEFAULT_WS281X_COLOR_LED_ON;
	saveConfig();
	Log.println("configuration restored.");
}

/********************************************************/
/******************** Private Method ********************/
/********************************************************/

#if !defined(NO_GLOBAL_INSTANCES) 
EEPROMConfiguration Configuration;
#endif
