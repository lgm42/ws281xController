#include <EEPROM.h>

#include <ArduinoJson.h>

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
	Log.println(String("    mqttServerIp: ") + String(_data.mqttServerIp));
	Log.println(String("    mqttServerPort: ") + String(_data.mqttServerPort));
	Log.println(String("    mqttLogin: ") + String(_data.mqttLogin));
	Log.println(String("    mqttPassword: ") + String(_data.mqttPassword));
	Log.println(String("    colorLedOn: ") + String(_data.colorLedOn));
	Log.println(String("    numLeds: ") + String(_data.numLeds));
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

String EEPROMConfiguration::mqttServerIp()
{
    return String(_data.mqttServerIp);
}

uint16_t EEPROMConfiguration::mqttServerPort()
{
    return _data.mqttServerPort;
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

CommandLine EEPROMConfiguration::getCommandLineFromId(const int id)
{
  if (id >= CommandLine::kEndOfCommandLineIds)
  {
    Log.println("Command line id requested to high: " + String(id) + ", should be less than: " + String(CommandLine::kEndOfCommandLineIds));
    return CommandLine(-1, "", "");
  }

  return CommandLine(id, String(_data.commandLines[id].name), String(_data.commandLines[id].command));
}

void EEPROMConfiguration::updateCommandLine(const CommandLine & commandLine)
{
  if (commandLine.id() >= CommandLine::kEndOfCommandLineIds)
  {
    Log.println("Command line id requested to high: " + String(commandLine.id()) + ", should be less than: " + String(CommandLine::kEndOfCommandLineIds));
  }

  Log.println("#" + commandLine.command() + "#");

  commandLine.name().toCharArray(_data.commandLines[commandLine.id()].name, COMMAND_LINE_MAX_NAME_LENGTH);
  commandLine.command().toCharArray(_data.commandLines[commandLine.id()].command, COMMAND_LINE_MAX_COMMAND_LENGTH);
  saveConfig();
}

void EEPROMConfiguration::restoreDefault()
{  
  _data.eepromValidity = EXPECTED_EEPROM_VALIDITY;
  strcpy(_data.hostname, DEFAULT_HOSTNAME);
  strcpy(_data.mqttServerIp, DEFAULT_MQTTSERVERIP); 
  _data.mqttServerPort = DEFAULT_MQTTSERVERPORT; 
  strcpy(_data.mqttLogin, "");
  strcpy(_data.mqttPassword, "");
  _data.numLeds = DEFAULT_WS281X_LED_COUNT;
  _data.colorLedOn = DEFAULT_WS281X_COLOR_LED_ON;

  for (int i = 0; i < CommandLine::kEndOfCommandLineIds; ++i)
  {
    updateCommandLine(CommandLine(i, "", ""));
  }

  updateCommandLine(CommandLine(CommandLine::kBtOnActionCommandLineId, "btOn", "ws_animate=0;ws_fadeto=FFFFFF"));
  updateCommandLine(CommandLine(CommandLine::kBtOffActionCommandLineId, "btOff", "ws_animate=0;ws_fadeto=000000"));
  updateCommandLine(CommandLine(CommandLine::kFadeInActionCommandLineId, "fadeIn", "ws_animate=0;ws_brightness=p10"));
  updateCommandLine(CommandLine(CommandLine::kFadeOutActionCommandLineId, "fadeOut", "ws_animate=0;ws_brightness=m10"));
  
  updateCommandLine(CommandLine(CommandLine::kCustom0ActionCommandLineId, "Liseuse", "ws_animate=0;ws_fadesetpixelto=75_90_FFFFFF"));
  
  
	saveConfig();
	Log.println("configuration restored.");
}

String EEPROMConfiguration::toJson()
{
  DynamicJsonDocument conf(4096);

  conf["hostname"] = hostname();
  conf["mqttServerIp"]   = mqttServerIp();
  conf["mqttServerPort"] = mqttServerPort();
  conf["mqttLogin"] = mqttLogin();
  conf["mqttPassword"] = mqttPassword();
  conf["colorLedOn"] = colorLedOn();
  conf["numLeds"] = numLeds();

  for (int id = 0; id < CommandLine::kEndOfCommandLineIds; ++id)
  {
    conf["customCommands"][String(id)]["name"] = _data.commandLines[id].name;
    conf["customCommands"][String(id)]["command"] = _data.commandLines[id].command;
  }

  String output;  
  serializeJson(conf, output);
  return output;
}

void EEPROMConfiguration::fromJson(const String & json)
{
  Log.println(json);
  DynamicJsonDocument conf(1024);
  deserializeJson(conf, json);

  Log.println("deserialized");

  const char* tmp;
  tmp = conf["hostname"].as<const char*>();
  memset(_data.hostname, 0, 30);
  strncpy(_data.hostname, tmp, min((int)strlen(tmp), 30));

  tmp = conf["mqttServerIp"].as<const char*>();
  memset(_data.mqttServerIp, 0, 30);
  strncpy(_data.mqttServerIp, tmp, min((int)strlen(tmp), 30));

  _data.mqttServerPort = conf["mqttServerPort"];

  tmp = conf["mqttLogin"].as<const char*>();
  memset(_data.mqttLogin, 0, 30);
  strncpy(_data.mqttLogin, tmp, min((int)strlen(tmp), 30));

  tmp = conf["mqttPassword"].as<const char*>();
  memset(_data.mqttPassword, 0, 30);
  strncpy(_data.mqttPassword, tmp, min((int)strlen(tmp), 30));

  _data.colorLedOn = conf["colorLedOn"];
  _data.numLeds = conf["numLeds"];

  for (int id = 0; id < CommandLine::kEndOfCommandLineIds; ++id)
  {
    if (conf["customCommands"].containsKey(String(id)))
    {
      if (conf["customCommands"][String(id)].containsKey("name"))
      {
        tmp = conf["customCommands"][String(id)]["name"].as<const char*>();
        memset(_data.commandLines[id].name, 0, 30);
        strncpy(_data.commandLines[id].name, tmp, min((int)strlen(tmp), 30));
      }

      if (conf["customCommands"][String(id)].containsKey("command"))
      {
        tmp = conf["customCommands"][String(id)]["command"].as<const char*>();
        memset(_data.commandLines[id].command, 0, 135);
        strncpy(_data.commandLines[id].command, tmp, min((int)strlen(tmp), 135));
      }
    }
  }

  saveConfig();
}

/********************************************************/
/******************** Private Method ********************/
/********************************************************/

#if !defined(NO_GLOBAL_INSTANCES) 
EEPROMConfiguration Configuration;
#endif
