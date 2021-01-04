#pragma once

#include "EEPROMConfiguration.h"

class ActionManager
{
public:
	ActionManager();
	virtual ~ActionManager();
    String manageCommandLine(const String & commandLine);
    String manageCommand(const String & name, const String & value);

    String executeBtnOnAction();
    String executeBtnOffAction();
    String executeDimmerFadeInAction();
    String executeDimmerFadeOutAction();
    String executeBtnDoubleTouchAction();

private:

};

#if !defined(NO_GLOBAL_INSTANCES)
extern ActionManager ActionRunner;
#endif

