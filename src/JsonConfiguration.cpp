#include <LittleFS.h>
#include <ArduinoJson.h>

#include "Logger.h"
#include "JsonConfiguration.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

JsonConfiguration::JsonConfiguration()
{
}

JsonConfiguration::~JsonConfiguration()
{
}

void JsonConfiguration::setup(void)
{
  /* Initialize LittleFS */
  if (!LittleFS.begin()) {
    Log.println("failed to initialize LittleFS, try to format");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Log.println("definitely failed to initialize LittleFS !");
      return;
    }
  }

	if (!readConfig()) {
		Log.println("Invalid configuration values, restoring default values");
		restoreDefault();
	}

	Log.println(String("    hostname: ") + _hostname);
	Log.println(String("    mqttIpServer: ") + _mqttIpServer);
	Log.println(String("    mqttPortServer: ") + String(_mqttPortServer));
	Log.println(String("    numLeds: ") + String(_numLeds));
	Log.println(String("    neoPixelType: ") + String(_neoPixelType));
}

bool JsonConfiguration::readConfig()
{
  Log.println("Read Configuration file from LittleFS...");

  // Open file
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) 
  {
    Log.println("Failed to open config file");
    return false;
  }
  
  // Allocate a buffer to store contents of the file.
  // StaticJsonDocument<512> doc;
  DynamicJsonDocument doc(512);
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Log.println(F("Failed to read file, using default configuration"));
  }
  
  _hostname           = doc["hostname"] | DEFAULT_HOSTNAME;
  _mqttIpServer       = doc["mqttIpServer"] | DEFAULT_MQTTIPSERVER; 
  _mqttPortServer     = doc["mqttPortServer"] | DEFAULT_MQTTPORTSERVER; 
  _numLeds          = doc["numLeds"] | DEFAULT_WS281X_LED_COUNT; 
  _neoPixelType     = doc["neoPixelType"] | DEFAULT_WS281X_NEO_PIXEL_TYPE; 
  
  configFile.close();

  return true;
}

bool JsonConfiguration::saveConfig()
{
  // StaticJsonDocument<512> doc;
  DynamicJsonDocument doc(512);
 
	doc["hostname"]               = _hostname;
  doc["mqttIpServer"]           = _mqttIpServer;
  doc["mqttPortServer"]         = _mqttPortServer;
  doc["numLeds"]         = _numLeds;
  doc["neoPixelType"]         = _neoPixelType;
  
	File configFile = LittleFS.open("/config.json", "w");
	if (!configFile) {
		Log.println("Failed to open config file for writing");
		return false;
	}
  
  // Serialize JSON to file
  if (serializeJson(doc, configFile) == 0) {
    Log.println(F("Failed to write to file"));
    return false;
  }

  configFile.close();

  
	Log.println("Save config successfully");
	Log.println(String("    hostname: ") + _hostname);
	Log.println(String("    mqttIpServer: ") + _mqttIpServer);
	Log.println(String("    mqttPortServer: ") + String(_mqttPortServer));
	Log.println(String("    numLeds: ") + String(_numLeds));
	Log.println(String("    neoPixelType: ") + String(_neoPixelType));
	
	return true;
}

void JsonConfiguration::restoreDefault()
{  
	_hostname           = DEFAULT_HOSTNAME;
  _mqttIpServer       = DEFAULT_MQTTIPSERVER; 
  _mqttPortServer     = DEFAULT_MQTTPORTSERVER; 
  _numLeds = DEFAULT_WS281X_LED_COUNT;
  _neoPixelType = DEFAULT_WS281X_NEO_PIXEL_TYPE;

	saveConfig();
	Log.println("configuration restored.");
}

/********************************************************/
/******************** Private Method ********************/
/********************************************************/

#if !defined(NO_GLOBAL_INSTANCES) 
JsonConfiguration Configuration;
#endif
