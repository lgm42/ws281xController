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

String ActionManager::manageCommandLineFromId(const int id)
{
    CommandLine commandLine = Configuration.getCommandLineFromId(id);
    return manageCommandLine(commandLine.command());
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
    else if (nameToUse == "bt_on")
        return executeBtnOnAction();
    else if (nameToUse == "bt_off")
        return executeBtnOffAction();
    else if (nameToUse == "bt_doubleTap")
        return executeBtnDoubleTouchAction();
    return "";
}

String ActionManager::executeBtnOnAction()
{
    Log.println("Executing On Action");
    return manageCommandLine(Configuration.getCommandLineFromId(CommandLine::kBtOnActionCommandLineId).command());
}

String ActionManager::executeBtnOffAction()
{
    Log.println("Executing Off Action");
    return manageCommandLine(Configuration.getCommandLineFromId(CommandLine::kBtOffActionCommandLineId).command());
}

String ActionManager::executeDimmerFadeInAction()
{
    Log.println("Executing Fade In Dimmer Action");
    return manageCommandLine(Configuration.getCommandLineFromId(CommandLine::kFadeInActionCommandLineId).command());
}

String ActionManager::executeDimmerFadeOutAction()
{
    Log.println("Executing Fade Out Dimmer Action");
    return manageCommandLine(Configuration.getCommandLineFromId(CommandLine::kFadeOutActionCommandLineId).command());
}

String ActionManager::executeBtnDoubleTouchAction()
{
    return manageCommandLine(Configuration.getCommandLineFromId(CommandLine::kCustom0ActionCommandLineId).command());
}

#if !defined(NO_GLOBAL_INSTANCES)
ActionManager ActionRunner;
#endif
