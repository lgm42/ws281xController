#pragma once

#define DEBUG_TELNET
#define DEBUG_BY_TELNET
#define DEBUG_BY_SERIAL

#define PWM_MAX	255
#define PWM_MIN	0

#include <Arduino.h>
#include <WS2812FX.h>

class WS281xDriver 
{
  public:
  
    static const int FadeAnimationIndex;
    static const int FadeBrightnessAnimationIndex;

	WS281xDriver() {}
  	virtual ~WS281xDriver() {}

	void setup();
	void handle();

	WS2812FX * driver();

	String sendCommand(const String & name, const String & value);
	String sendCommand(const String & command);

	bool isBrightnessMin();
	bool isBrightnessMax();

	uint8_t getWhiteColor() const;
	uint8_t getRedColor() const;
	uint8_t getGreenColor() const;
	uint8_t getBlueColor() const;

	void setWhiteColor(const uint8_t value);
	void setRedColor(const uint8_t value);
	void setGreenColor(const uint8_t value);
	void setBlueColor(const uint8_t value);


	static void getPixelColor(const uint32_t color, uint16_t & n, uint8_t & r, uint8_t & g, uint8_t & b);
	static uint8_t getWhitePixelColor(const uint32_t color);
	static uint8_t getRedPixelColor(const uint32_t color);
	static uint8_t getGreenPixelColor(const uint32_t color);
	static uint8_t getBluePixelColor(const uint32_t color);

  private:
  WS2812FX * _ws2812fx;
  uint32_t _fadeSourceColor;
  uint32_t _fadeDestColor;
  uint8_t _fadeCurrentBrightness;
  uint8_t _fadeDestBrightness;

  static const uint8_t BrightnessGammaTable[101];

  int parseOffset(const String & value);
  static uint16_t fadeIn(void);
  static uint16_t fadeBrightness(void);

};

#if !defined(NO_GLOBAL_INSTANCES)
extern WS281xDriver LedDriver;
#endif
