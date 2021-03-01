
#include "NeoPixelBus.h"

#include "Logger.h"

#include "Animations.h"
#include "WS281xDriver.h"

#include "EEPROMConfiguration.h"

namespace animation
{
    AnimUpdateCallback getAnimationCallback(const int index)
    {
        switch (index)
        {
            default:
            case FadingColorsAnimationIndex:
                return fadingColorsAnimationUpdate;
            case LedsRunningAnimationIndex:
                return ledsRunningAnimationUpdate;
            case FillingColorsAnimationIndex:
                return fillingColorsAnimationUpdate;
        }
    }

    void fadingColorsAnimationUpdate(const AnimationParam& param)
    {
        if (param.state == AnimationState_Started)
        {
            //we initialize animation parameters
            LedDriver.animationParameters().startingColor = LedDriver.strip()->GetPixelColor(0);
            LedDriver.animationParameters().endingColor = HslColor(random(360) / 360.0f, 1.0f, ANIMATION_LUMINANCE);
        }

        RgbColor updatedColor = RgbColor::LinearBlend(
                LedDriver.animationParameters().startingColor,
                LedDriver.animationParameters().endingColor,
                param.progress);

        LedDriver.strip()->ClearTo(updatedColor);
        delay(1);
        
        if (param.state == AnimationState_Completed)
        {
            //we restart animation with new params
            LedDriver.animator()->RestartAnimation(FadingColorsAnimationIndex);
        }
    }

    void ledsRunningAnimationUpdate(const AnimationParam& param)
    {
        if (param.state == AnimationState_Started)
        {
             Log.println("Animation started");
            //we initialize animation parameters
            LedDriver.animationParameters().startingColor = HslColor(random(360) / 360.0f, 1., ANIMATION_LUMINANCE);

            //We set the first pixel to a random color
            LedDriver.strip()->ClearTo(RgbColor(0));
        }

        int ledIndex = Configuration.numLeds() * param.progress;

        for (int i = 0; i < Configuration.numLeds(); ++i)
        {
            LedDriver.strip()->SetPixelColor(i, LedDriver.strip()->GetPixelColor(i).Dim(192));
            delay(1);
        }

        LedDriver.strip()->SetPixelColor(ledIndex, LedDriver.animationParameters().startingColor);
        

        if (param.state == AnimationState_Completed)
        {
            //we restart animation with new params
            LedDriver.animator()->RestartAnimation(LedsRunningAnimationIndex);
        }
    }

    void fillingColorsAnimationUpdate(const AnimationParam& param)
    {
        static int lastIndex;
        if (param.state == AnimationState_Started)
        {
            //we initialize animation parameters
            LedDriver.animationParameters().startingColor = HslColor(random(360) / 360.0f, 1., ANIMATION_LUMINANCE);
            lastIndex = 0;
        }

        int ledIndex = Configuration.numLeds() * param.progress;

        LedDriver.strip()->ClearTo(LedDriver.animationParameters().startingColor, lastIndex, ledIndex);
        delay(1);

        lastIndex = ledIndex;

        if (param.state == AnimationState_Completed)
        {
            //we restart animation with new params
            LedDriver.animator()->RestartAnimation(FillingColorsAnimationIndex);
        }
    }

} // namespace animation
