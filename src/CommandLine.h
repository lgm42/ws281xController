#pragma once

#include "settings.h"

#define COMMAND_LINE_MAX_NAME_LENGTH  30
#define COMMAND_LINE_MAX_COMMAND_LENGTH 135

class CommandLine 
{
  public:
    typedef enum
    {
        kBtOnActionCommandLineId = 0x00,
        kBtOffActionCommandLineId,
        kFadeInActionCommandLineId,
        kFadeOutActionCommandLineId,
        kCustom0ActionCommandLineId,
        kCustom1ActionCommandLineId,
        kCustom2ActionCommandLineId,
        kCustom3ActionCommandLineId,
        kCustom4ActionCommandLineId,
        kCustom5ActionCommandLineId,
        kCustom6ActionCommandLineId,
        kCustom7ActionCommandLineId,
        kCustom8ActionCommandLineId,
        kCustom9ActionCommandLineId,

        kEndOfCommandLineIds
    } CommandLineId;

  	CommandLine(const int id, const String & name, const String & command)
      : _id(id), _name(name), _command(command)
    {
    }

  	virtual ~CommandLine()
    {
    }
    
    int id() const
    {
      return _id;
    }

    const String & name() const
    {
        return _name;
    }

    void name(const String & value)
    {
        _name = value;
    }
    
    const String & command() const
    {
        return _command;
    }

    void command(const String & value)
    {
        _command = value;
    }

  private:
    int _id;
    String _name;
    String _command;
};