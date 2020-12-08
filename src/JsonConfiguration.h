#pragma once

#include "settings.h"

class JsonConfiguration 
{
  public:
  	JsonConfiguration();
  	virtual ~JsonConfiguration();
  
  	void setup();

    bool readConfig();
  	bool saveConfig();
  
  	void restoreDefault();
    
    String toJson();
  
	/* Members */
  	String _hostname;
	String _mqttIpServer;
	uint16_t _mqttPortServer;
	uint16_t _numLeds;
	uint16_t _neoPixelType;

  private:
  
};

#if !defined(NO_GLOBAL_INSTANCES)
extern JsonConfiguration Configuration;
#endif
