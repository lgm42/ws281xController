#pragma once

#include "EEPROMConfiguration.h"

class BtnManager
{
public:
	BtnManager();
	virtual ~BtnManager();

	void setup(void);
	void handle(void);

    bool btnPressed() const;

private:
    void runPressCommand(const int nbPress);
    void runDimmerCommand();

    typedef enum EState
    {
        kWaitForPress,
        kWaitForRelease,
        kWaitForEndOfDimmer,
        kWaitForNextPress
    };

    bool _lastButtonState;
    unsigned long _lastButtonChangeTime;
    EState _fsmState;
    int _nbPressed;
    bool _fadeIn;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern BtnManager ButtonManager;
#endif

