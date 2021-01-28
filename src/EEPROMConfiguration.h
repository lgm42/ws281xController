#pragma once

#include "settings.h"

#include "CommandLine.h"

#define CUSTOM_COMMAND_NUMBER 	10

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
	void fromJson(const String & json);
  
	String hostname();
	String mqttServerIp();
	uint16_t mqttServerPort();
	String mqttLogin();
	String mqttPassword();
	uint32_t colorLedOn();
	uint16_t numLeds();
	uint16_t neoPixelType();
	CommandLine getCommandLineFromId(const int id);
	void updateCommandLine(const CommandLine & commandLine);

  private:

	typedef struct commandLineContent
	{
		char name[COMMAND_LINE_MAX_NAME_LENGTH];
		char command[COMMAND_LINE_MAX_COMMAND_LENGTH];
	};
	
  typedef struct 
	{
		uint32_t eepromValidity;
		char hostname[30];
		char mqttServerIp[30];
		uint16_t mqttServerPort;
		char mqttLogin[30];
		char mqttPassword[30];
		uint32_t colorLedOn;
		uint16_t numLeds;
		uint16_t neoPixelType;
		commandLineContent commandLines[CUSTOM_COMMAND_NUMBER + 4];
	} rawData;

	rawData _data;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern EEPROMConfiguration Configuration;
#endif
