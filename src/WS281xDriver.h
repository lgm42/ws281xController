#pragma once

#define DEBUG_TELNET
#define DEBUG_BY_TELNET
#define DEBUG_BY_SERIAL

#define PWM_MAX	255
#define PWM_MIN	0

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#include "animation/Animations.h"
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

	animation::AnimationParameters & animationParameters();

	NeoPixelAnimator * animator();

#ifdef LED_INVERTED
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812InvertedMethod> * strip();
#else
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812Method> * strip();
#endif

private:

#ifdef LED_INVERTED
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812InvertedMethod> * _neoPixelBus;
#else
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812Method> * _neoPixelBus;
#endif

	NeoPixelAnimator * _animator;

	RgbColor _globalCurrentColor;
	int _animationSpeed;
	animation::AnimationParameters _animationParameters;
	int _animationRunningIndex;

	//Animation Description

	static const int FadeToAnimationIndex = 0;

	int parseOffset(const String & value);

	static void fadeAnimationUpdate(const AnimationParam& param);

};

#if !defined(NO_GLOBAL_INSTANCES)
extern WS281xDriver LedDriver;
#endif
