
#include "Logger.h"
#include "EEPROMConfiguration.h"

#include "WS281xDriver.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

void WS281xDriver::setup()
{
  //NeoEsp8266Uart1Ws2812InvertedMethod
	_neoPixelBus = new NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812Method>(Configuration.numLeds());
	_neoPixelBus->ClearTo(RgbColor(0));
	_animator = new NeoPixelAnimator(AnimationCount);
	_neoPixelBus->Begin();
	_neoPixelBus->Show();

  	_globalCurrentColor = RgbColor(0);
	_animationSpeed = 2000;
}

void WS281xDriver::handle()
{
	_animator->UpdateAnimations();
    _neoPixelBus->Show();
}

int WS281xDriver::parseOffset(const String & value)
{
  if (value.startsWith("p"))
    return value.substring(1).toInt();
  else if (value.startsWith("m"))
    return -1 * value.substring(1).toInt();
  //not an offset
  return 0;
}

String WS281xDriver::sendCommand(const String & command)
{
    int startCommandIndex = 0;
    while (startCommandIndex > -1)
    {
        int next = command.indexOf(',');
        if (next == -1)
            next = command.length() - 1;

        int separatorIndex = command.indexOf('=', startCommandIndex);
        if (separatorIndex != -1)
        {
            String name = command.substring(startCommandIndex, separatorIndex - 1);
            String value = command.substring(separatorIndex + 1, next);
            sendCommand(name, value);
        }

        startCommandIndex = next;
    }
    return "";
}

//Used to change define absolute brightness to a color
RgbColor WS281xDriver::setBrightness(RgbColor source, uint8_t brightness)
{
	//we can't set brightness to black so if source is black we set it as default Color
	if (source == RgbColor(0))
		source = RgbColor(Configuration.colorLedOn());

	int brightnessSource = source.CalculateBrightness();

	Log.println(String("set brightness to ") + String(brightness));

	if (brightness > brightnessSource)
	{
		//Brighten
		int brightenRatio = 255 - (((brightness - brightnessSource) * 255) / (255 - brightnessSource));
		return source.Brighten(brightenRatio);
	}
	else
	{
		//Dim
		int dimRatio = (brightness * 255) / brightnessSource;
		return source.Dim(dimRatio);
	}
}


String WS281xDriver::sendCommand(const String & name, const String & value)
{
    String nameToUse(name);
    nameToUse.toLowerCase();
    String valueToUse(value);
    valueToUse.toLowerCase();

    Log.println(String("command ") + name + String(" : ") + value);

    if (nameToUse == "ws_fadeto")
    {
		uint32_t destColor = strtol(valueToUse.c_str(), 0, 16);
		Log.println("dest color : " + String(destColor));
		//we copy current led colors into a starting array (should be optimized with a memcpy)
		for (int i = 0; i < Configuration.numLeds(); ++i)
		{
			_animationState.startingLedColors[i] = _neoPixelBus->GetPixelColor(i);
			_animationState.endingLedColors[i] = HtmlColor(destColor);
		}
		_animationState.easeing = NeoEase::CubicOut;

		Log.println(String("set fade to ") + String(destColor));
		_animator->StartAnimation(FadeToAnimationIndex, _animationSpeed, fadeAnimationUpdate);
      
    }
    else if (nameToUse == "ws_fadebrightnessto")
    {
		int offset = parseOffset(valueToUse);
		Log.println("dest brightness : " + valueToUse);
		
		_animationState.easeing = NeoEase::CubicOut;
		
		//we re-use the fade animation computing target color of each pixel
		for (int i = 0; i < Configuration.numLeds(); ++i)
		{
			_animationState.startingLedColors[i] = _neoPixelBus->GetPixelColor(i);
			uint8_t destBrightness;
			//it can be a relative offset if defined else an absolute value
			if (offset != 0)
			{
				destBrightness = _animationState.startingLedColors[i].CalculateBrightness() + offset;
			}
			else
			{
				destBrightness = valueToUse.toInt();
			}
			_animationState.endingLedColors[i] = setBrightness(_animationState.startingLedColors[i], destBrightness);
		}

		_animator->StartAnimation(FadeToAnimationIndex, _animationSpeed, fadeAnimationUpdate);
    }
    else if (nameToUse == "ws_brightness")
    {
		int offset = parseOffset(valueToUse);
		int target;

		for (int i = 0; i < Configuration.numLeds(); ++i)
		{
			if (offset != 0)
			{
				target = _neoPixelBus->GetPixelColor(i).CalculateBrightness() + offset;
			}
			else
			{
				target = valueToUse.toInt();
			}

			target = max(min((int)255, target), (int)0);

			RgbColor newColor = setBrightness(_neoPixelBus->GetPixelColor(i), target);
			_neoPixelBus->SetPixelColor(i, newColor);
		}
    }
    else if (nameToUse == "ws_color")
    {
		uint32_t destColor = strtol(valueToUse.c_str(), 0, 16);
		Log.println("dest color : " + String(destColor));
		_neoPixelBus->ClearTo(HtmlColor(destColor));
    }
    else if (nameToUse == "ws_speed")
    {
		int offset = parseOffset(valueToUse);
		int val;
		if (offset != 0)
		{
			val = _animationSpeed + offset;
		}
		else
		{
			val = valueToUse.toInt();
		}

		val = max(min((int)SPEED_MAX, val), (int)SPEED_MIN);

		Log.println(String("set WS2812FX speed to ") + String(val));
		_animationSpeed = val;
    }
	/*
    else if (nameToUse == "ws_pwm0")
    {
      int offset = parseOffset(valueToUse);
      int val;
      if (offset != 0)
      {
        val = LedDriver.getWhiteColor() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }

      val = max(min((int)PWM_MAX, val), (int)PWM_MIN);

      Log.println(String("set WS2812FX White color to ") + String(val));
      LedDriver.setWhiteColor(val);
    }
    else if (nameToUse == "ws_pwm1")
    {
      int offset = parseOffset(valueToUse);
      int val;
      if (offset != 0)
      {
        val = LedDriver.getRedColor() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }

      val = max(min((int)PWM_MAX, val), (int)PWM_MIN);

      Log.println(String("set WS2812FX Red color to ") + String(val));
      LedDriver.setRedColor(val);
    }
    else if (nameToUse == "ws_pwm2")
    {
      int offset = parseOffset(valueToUse);
      int val;
      if (offset != 0)
      {
        val = LedDriver.getGreenColor() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }

      val = max(min((int)PWM_MAX, val), (int)PWM_MIN);

      Log.println(String("set WS2812FX Green color to ") + String(val));
      LedDriver.setGreenColor(val);
    }
    else if (nameToUse == "ws_pwm3")
    {
      int offset = parseOffset(valueToUse);
      int val;
      if (offset != 0)
      {
        val = LedDriver.getBlueColor() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }

      val = max(min((int)PWM_MAX, val), (int)PWM_MIN);

      Log.println(String("set WS2812FX blue color to ") + String(val));
      LedDriver.setBlueColor(val);
    }*/
    else if (nameToUse == "ws_setpixel")
    {
		//split params into multiple ones
		//ie: ws_setPixel=3_13_FF00FF
		int endOfStart = valueToUse.indexOf("_");
		int endOfEnd = valueToUse.indexOf("_", endOfStart + 1);

		int start = valueToUse.substring(0, endOfStart ).toInt();
		int end = valueToUse.substring(endOfStart + 1, endOfEnd).toInt();
		HtmlColor destColor(strtol(valueToUse.c_str() + endOfEnd + 1, 0, 16));

		char logBuffer[10];
		destColor.ToNumericalString(logBuffer, 10);
		Log.println(String("set WS2812FX led from ") + String(start) + " to " + String(end) + " to color " + String(logBuffer));
		for (int n = start; n <= end; ++n)
			_neoPixelBus->SetPixelColor(n, destColor);
    }
	else if (nameToUse == "ws_fadesetpixelto")
    {
		//split params into multiple ones
		//ie: ws_setPixel=3_13_FF00FF
		int endOfStart = valueToUse.indexOf("_");
		int endOfEnd = valueToUse.indexOf("_", endOfStart + 1);

		int start = valueToUse.substring(0, endOfStart ).toInt();
		int end = valueToUse.substring(endOfStart + 1, endOfEnd).toInt();
		HtmlColor destColor(strtol(valueToUse.c_str() + endOfEnd + 1, 0, 16));

		char logBuffer[10];
		destColor.ToNumericalString(logBuffer, 10);
		Log.println(String("set fade led from ") + String(start) + " to " + String(end) + " to color " + String(logBuffer));
		
		//all pixels non concerned will be set to off

		//we copy current led colors into a starting array (should be optimized with a memcpy)
		for (int i = 0; i < Configuration.numLeds(); ++i)
		{
			_animationState.startingLedColors[i] = _neoPixelBus->GetPixelColor(i);
			if ((i >= start) && (i <= end))
				_animationState.endingLedColors[i] = destColor;
			else
				_animationState.endingLedColors[i] = RgbColor(0);
			
		}
		_animationState.easeing = NeoEase::CubicOut;

		_animator->StartAnimation(FadeToAnimationIndex, _animationSpeed, fadeAnimationUpdate);

    }
    return "";
}

uint8_t WS281xDriver::globalBrightness() const
{
	uint32_t sum = 0;

	for (int i = 0; i < Configuration.numLeds(); ++i)
    {
        sum += _neoPixelBus->GetPixelColor(i).CalculateBrightness();
    }

    return (uint8_t)(sum / Configuration.numLeds());
}

bool WS281xDriver::isBrightnessMin() const
{
  return globalBrightness() == 0;
}

bool WS281xDriver::isBrightnessMax() const
{
  return globalBrightness() == 255;
}

void WS281xDriver::fadeAnimationUpdate(const AnimationParam& param)
{
    // first apply an easing (curve) to the animation
    // this simulates acceleration to the effect
    float progress = LedDriver._animationState.easeing(param.progress);

    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
	//for each pixel
	for (int i = 0; i < Configuration.numLeds(); ++i)
	{
      RgbColor updatedColor = RgbColor::LinearBlend(
          LedDriver._animationState.startingLedColors[i],
          LedDriver._animationState.endingLedColors[i],
          progress);
      
      // apply the color to the strip
      LedDriver._neoPixelBus->SetPixelColor(i, updatedColor);
	}
}

void WS281xDriver::fadeBrightnessAnimationUpdate(const AnimationParam& param)
{
    // first apply an easing (curve) to the animation
    // this simulates acceleration to the effect
    float progress = LedDriver._animationState.easeing(param.progress);

    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
	//for each pixel
	for (int i = 0; i < Configuration.numLeds(); ++i)
	{
		uint8_t newRatio = LedDriver._animationState.param0PerLed[i] + (LedDriver._animationState.endingBrightness - LedDriver._animationState.param0PerLed[i]) * progress;
		if (i == 0)
		{
			Log.println("new ratio : " + String(newRatio));
			Log.println("LedDriver._animationState.param0PerLed[i] : " + String(LedDriver._animationState.param0PerLed[i]));
			Log.println("LedDriver._animationState.endingBrightness : " + String(LedDriver._animationState.endingBrightness));
			Log.println("progress : " + String(progress));
			char tmp[30];
			HtmlColor(LedDriver._neoPixelBus->GetPixelColor(i)).ToNumericalString(tmp, 30);
			Log.println("LedDriver._neoPixelBus->GetPixelColor(i) : " + String(tmp));

			HtmlColor(LedDriver._neoPixelBus->GetPixelColor(i).Brighten(newRatio)).ToNumericalString(tmp, 30);
			Log.println("LedDriver._neoPixelBus->GetPixelColor(i).Brighten(newRatio) : " + String(tmp));
		}
		LedDriver._neoPixelBus->SetPixelColor(i, LedDriver._neoPixelBus->GetPixelColor(i).Dim(newRatio));
	}
}

#if !defined(NO_GLOBAL_INSTANCES) 
WS281xDriver LedDriver;
#endif
