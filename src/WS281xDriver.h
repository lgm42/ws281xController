#pragma once

#define DEBUG_TELNET
#define DEBUG_BY_TELNET
#define DEBUG_BY_SERIAL

#define PWM_MAX	255
#define PWM_MIN	0

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#include "settings.h"

class WS281xDriver 
{
  public:

	WS281xDriver() {}
  	virtual ~WS281xDriver() {}

	void setup();
	void handle();

	String sendCommand(const String & name, const String & value);
	String sendCommand(const String & command);

	uint8_t globalBrightness() const;
	bool isBrightnessMin() const;
	bool isBrightnessMax() const;

	//Used to change define absolute brightness to a color
	static RgbColor setBrightness(RgbColor source, uint8_t brightness);

private:
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812Method> * _neoPixelBus;
	NeoPixelAnimator * _animator;

	//Mutualised parameters for animations
	struct AnimationState
	{
		AnimEaseFunction easeing; // the acceleration curve it will use 
		RgbColor startingLedColors[MAX_LED_COUNT];
		RgbColor endingLedColors[MAX_LED_COUNT];

		uint8_t param0PerLed[MAX_LED_COUNT];

		RgbColor startingColor;  // the color the animation starts at
		RgbColor endingColor; // the color the animation will end at

		uint8_t endingBrightness;
	};

	RgbColor _globalCurrentColor;
	int _animationSpeed;

	//Animation Description

	static const int FadeToAnimationIndex = 0;
	static const int FadeBrightnessToAnimationIndex = 1;

	static const int AnimationCount = 2;
	AnimationState _animationState;

	int parseOffset(const String & value);

	static void fadeAnimationUpdate(const AnimationParam& param);
	static void fadeBrightnessAnimationUpdate(const AnimationParam& param);

};

#if !defined(NO_GLOBAL_INSTANCES)
extern WS281xDriver LedDriver;
#endif
