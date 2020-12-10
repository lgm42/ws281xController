#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>

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

protected:
  void sendJson(const uint16 code, JsonDocument &doc);

private:

  ESP8266WebServer          _webServer;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern HttpServer HTTPServer;
#endif

