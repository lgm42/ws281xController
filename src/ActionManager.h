#pragma once

#include "EEPROMConfiguration.h"

class ActionManager
{
public:
	ActionManager();
	virtual ~ActionManager();
    String manageCommandLineFromId(const int id);
    String manageCommandLine(const String & commandLine);
    String manageCommand(const String & name, const String & value);

    String executeBtnOnAction();
    String executeBtnOffAction();
    String executeDimmerFadeInAction();
    String executeDimmerFadeOutAction();
    String executeBtnDoubleTouchAction();

private:
    int customActionNumber;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern ActionManager ActionRunner;
#endif

