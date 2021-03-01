#pragma once

#include "NeoPixelAnimator.h"

#include "settings.h"

namespace animation
{
    typedef enum {
        FadingColorsAnimationIndex = 1,
        LedsRunningAnimationIndex,
        FillingColorsAnimationIndex,

        AnimationCount
    };

	//Mutualised parameters for animations
	typedef struct
	{
		AnimEaseFunction easeing; // the acceleration curve it will use 
		RgbColor startingLedColors[MAX_LED_COUNT];
		RgbColor endingLedColors[MAX_LED_COUNT];

		uint8_t param0PerLed[MAX_LED_COUNT];

		RgbColor startingColor;  // the color the animation starts at
		RgbColor endingColor; // the color the animation will end at

		uint8_t endingBrightness;
	} AnimationParameters;

    AnimUpdateCallback getAnimationCallback(const int index);

    void fadingColorsAnimationUpdate(const AnimationParam& param);
    void ledsRunningAnimationUpdate(const AnimationParam& param);
    void fillingColorsAnimationUpdate(const AnimationParam& param);

}