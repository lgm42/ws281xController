#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include "EEPROMConfiguration.h"

class HttpServer
{
public:
	HttpServer() ;
	virtual ~HttpServer();

	void setup(void);
	void handle(void);

	String getContentType(String filename);
  bool handleFileRead(String path);
  
  static void handleNotFound();
  static void handleCmd();

  ESP8266WebServer& webServer();
private:

  ESP8266WebServer          _webServer;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern HttpServer HTTPServer;
#endif

