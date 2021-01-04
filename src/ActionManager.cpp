#include <ESP8266mDNS.h>

#include "WiFiManager.h"
#include "Logger.h"
#include "WS281xDriver.h"

#include "ActionManager.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

ActionManager::ActionManager()
{
}

ActionManager::~ActionManager()
{
}

String ActionManager::manageCommandLine(const String & commandLine)
{
    Log.println("Executing command line: " + commandLine);
    //split commands into multiple ones
    //ie: ws_speed=100;bt_on;ws_fadebrightnessto=255
    String commandLineToParse = commandLine;
    if (not commandLineToParse.endsWith(";"))
        commandLineToParse += ";";

    int index = 0;
    int endOfCommand = commandLineToParse.indexOf(";");
    while (endOfCommand != -1)
    {
        String commandToParse(commandLineToParse.substring(index, endOfCommand));
        int equalityIndex = commandToParse.indexOf("=");
        String command;
        String params;
        if (equalityIndex == -1)
        {
            //simple command without params
            command = commandToParse;
            params = "";
        }
        else
        {
            //normal command (ie: ws_speed=1000)
            command = commandToParse.substring(0, equalityIndex);
            params = commandToParse.substring(equalityIndex + 1);
        }

        manageCommand(command, params);

        index = endOfCommand + 1;
        endOfCommand = commandLineToParse.indexOf(";", index);
    }
    
    return "";
}

String ActionManager::manageCommand(const String & name, const String & value)
{
    String nameToUse(name);
    nameToUse.toLowerCase();
    String valueToUse(value);
    valueToUse.toLowerCase();

    Log.println("Executing command " + name + " with params " + value);

    if (nameToUse.startsWith("ws_"))
      return LedDriver.sendCommand(nameToUse, valueToUse);
    //else if (nameToUse.startsWith("bt_"))
    //  return ButtonManager.sendCommand(nameToUse, valueToUse);
    else if (nameToUse == "bt_on")
        return executeBtnOnAction();
    else if (nameToUse == "bt_off")
        return executeBtnOffAction();
    return "";
}

String ActionManager::executeBtnOnAction()
{
    Log.println("Executing On Action");
    return manageCommandLine("ws_speed=1000;ws_fadebrightnessto=255");
}

String ActionManager::executeBtnOffAction()
{
    Log.println("Executing Off Action");
    return manageCommandLine("ws_speed=1000;ws_fadebrightnessto=0");
}

String ActionManager::executeDimmerFadeInAction()
{
    Log.println("Executing Fade In Dimmer Action");
    return manageCommandLine("ws_brightness=p10;ws_speed=1000");
}

String ActionManager::executeDimmerFadeOutAction()
{
    Log.println("Executing Fade Out Dimmer Action");
    return manageCommandLine("ws_brightness=m10");
}

String ActionManager::executeBtnDoubleTouchAction()
{
    return manageCommandLine("ws_brightness=255;ws_color=00FF00;ws_mode=50;ws_speed=10000");
}

#if !defined(NO_GLOBAL_INSTANCES)
ActionManager ActionRunner;
#endif
