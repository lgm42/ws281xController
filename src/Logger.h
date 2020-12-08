#pragma once

#define DEBUG_TELNET
#define DEBUG_BY_TELNET
#define DEBUG_BY_SERIAL

#include <Arduino.h>
#include <ESP8266WiFi.h>

class Logger 
{
  public:
	Logger() {}
  	virtual ~Logger() {}

	void setup();
	void handle();

	void println(const String &s = String());
	void println(const char str[]) { println(String(str)); }

	void print(const String &s);
	void print(const char str[]) { print(String(str)); }

  private:
	void send(String &s);
	void addTime(String &s);
#ifdef DEBUG_TELNET
	void handleTelnetClient();
#endif

	bool _addTimeToString;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern Logger Log;
#endif
