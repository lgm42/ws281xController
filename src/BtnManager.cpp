#include <littleFS.h>
#include <ESP8266mDNS.h>

#include "WiFiManager.h"
#include "Logger.h"
#include "WS281xDriver.h"

#include "BtnManager.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

BtnManager::BtnManager()
{
}

BtnManager::~BtnManager()
{
}

void BtnManager::setup(void)
{
    pinMode(BUTTON, INPUT_PULLUP); 
    _lastButtonState = btnPressed();
    _fsmState = kWaitForPress;
    _nbPressed = 0;
    _ledPowered = false;
    _fadeIn = true;
}

void BtnManager::handle(void)
{
    bool btnState = btnPressed();
    unsigned long now = millis();

    switch (_fsmState)
    {
        case kWaitForPress:
            if (btnState)
            {
                _lastButtonChangeTime = now;
                _fsmState = kWaitForRelease;
                Log.println("button pressed");
            }
        break;
        
        case kWaitForRelease:
            if (not btnState)
            {
                //We have release
                //did we press enough time or have we already make another short press ?
                if ((now - _lastButtonChangeTime > MIN_TIME_FOR_SHORT_PRESS) || (_nbPressed > 0))
                {
                    _fsmState = kWaitForNextPress;
                    _lastButtonChangeTime = now;
                    _nbPressed++;
                }
                else
                {
                    _fsmState = kWaitForPress;
                }
                
                Log.println("button released");
            }
            else
            {
                //we still press
                if (now - _lastButtonChangeTime > MIN_TIME_FOR_LONG_PRESS)
                {
                    Log.println("long press");
                    _fsmState = kWaitForEndOfDimmer;
                    _lastButtonChangeTime = now;
                }
            }
        break;
        
        case kWaitForEndOfDimmer:
            if (not btnState)
            {
                _nbPressed = 0;
                //we invert fade for next time
                _fadeIn = not _fadeIn;
                 _fsmState = kWaitForPress;
            }

            if (now - _lastButtonChangeTime > TIME_FOR_DIMMER_STEP)
            {
                runDimmerCommand();
                _lastButtonChangeTime = now;
            }
        break;
        
        case kWaitForNextPress:
            //we let a little time between two press
            if (btnState)
            {
                _lastButtonChangeTime = now;
                _fsmState = kWaitForRelease;
                Log.println("button pressed");
            }
            if (now - _lastButtonChangeTime > MAX_TIME_BETWEEN_TWO_PRESS)
            {
                Log.println("time expired");
                //action resolution based on number of press
                runPressCommand(_nbPressed);
                _nbPressed = 0;
                 _fsmState = kWaitForPress;
            }
        break;
    }


}

bool BtnManager::btnPressed() const
{
    return digitalRead(BUTTON);
}

void BtnManager::runPressCommand(const int nbPress)
{
    Log.println(String(nbPress) + String(" press"));
    
    switch (nbPress)
    {
        case 1:
            if (not _ledPowered)
            {
                //normal on
                LedDriver.sendCommand("ws_mode", "0");
                LedDriver.sendCommand("ws_brightness", "255");
                _fadeIn = false;
                _ledPowered = true;
            }
            else
            {
                LedDriver.sendCommand("ws_brightness", "0");
                _ledPowered = false;
            }
            
            break;
        case 2:
            LedDriver.sendCommand("ws_brightness", "255");
            LedDriver.sendCommand("ws_mode", "p1");
            _ledPowered = true;
            _fadeIn = false;
            break;
    }
}

void BtnManager::runDimmerCommand()
{
    Log.println("Dimmer action");
    if (_fadeIn)
    {
        LedDriver.sendCommand("ws_brightness", "p10");
    }
    else
    {
        LedDriver.sendCommand("ws_brightness", "m10");
    }
    
}

#if !defined(NO_GLOBAL_INSTANCES)
BtnManager ButtonManager;
#endif
