#include "Logger.h"
#include "EEPROMConfiguration.h"

#include "WS281xDriver.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

const int WS281xDriver::FadeAnimationIndex = 55;
const int WS281xDriver::FadeBrightnessAnimationIndex = 56;

const uint8_t WS281xDriver::BrightnessGammaTable[101] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 2,
  2, 2, 2, 2, 2, 3, 3, 4, 4, 4,
  5, 5, 5, 6, 6, 7, 7, 8, 8, 9,
  9, 10, 11, 11, 12, 13, 13, 15, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  27, 28, 29, 31, 32, 33, 35, 36, 37, 39,
  40, 42, 44, 45, 47, 48, 50, 51, 53, 55,
  57, 59, 60, 63, 64, 67, 68, 71, 72, 75,
  77, 79, 82, 84, 86, 88, 91, 93, 96, 99,
  100
};

void WS281xDriver::setup()
{
    _onColor = 0xFFFFFFFF;
    _ws2812fx = new WS2812FX(Configuration.numLeds(), WS281X_PIN, Configuration.neoPixelType());
    _ws2812fx->init();
    _ws2812fx->setBrightness(0);
    _ws2812fx->setSpeed(1000);
    _ws2812fx->setColor(_onColor);
    _ws2812fx->setMode(FX_MODE_STATIC);
    _ws2812fx->start();
    _fadeSourceColor = 0x000000;
    _fadeDestColor = _onColor;

    _ws2812fx->setCustomMode(F("Fade in"), fadeIn);
    _ws2812fx->setCustomMode(F("Fade brightness"), fadeBrightness);
}

void WS281xDriver::handle()
{
    _ws2812fx->service();
}

WS2812FX * WS281xDriver::driver()
{
    return _ws2812fx;
}

uint8_t WS281xDriver::getWhiteColor() const
{
    return (_ws2812fx->getColor() >> 24) & 0xFF;
}

uint8_t WS281xDriver::getRedColor() const
{
    return (_ws2812fx->getColor() >> 16) & 0xFF;
}

uint8_t WS281xDriver::getGreenColor() const
{
    return (_ws2812fx->getColor() >> 8) & 0xFF;
}

uint8_t WS281xDriver::getBlueColor() const
{
    return (_ws2812fx->getColor() >> 0) & 0xFF;
}

void WS281xDriver::setWhiteColor(const uint8_t value)
{
    _ws2812fx->setColor((value << 24) | (_ws2812fx->getColor() & 0x00FFFFFF));
}

void WS281xDriver::setRedColor(const uint8_t value)
{
    _ws2812fx->setColor((value << 16) | (_ws2812fx->getColor() & 0xFF00FFFF));
}

void WS281xDriver::setGreenColor(const uint8_t value)
{
    _ws2812fx->setColor((value << 8) | (_ws2812fx->getColor() & 0xFFFF00FF));
}

void WS281xDriver::setBlueColor(const uint8_t value)
{
    _ws2812fx->setColor((value << 0) | (_ws2812fx->getColor() & 0xFFFFFF00));
}

void WS281xDriver::getPixelColor(const uint32_t color, uint16_t & n, uint8_t & r, uint8_t & g, uint8_t & b) 
{
  n = (color >> 24) & 0xFF;
  r = (color >> 16) & 0xFF;
  g = (color >> 8) & 0xFF;
  b = (color >> 0) & 0xFF;
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
        //TODO : finish
    }
    return "";
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
      LedDriver._fadeDestColor = destColor;
      Log.println(String("set WS2812FX fade to ") + String(WS281xDriver::FadeAnimationIndex));
      LedDriver.driver()->setMode(WS281xDriver::FadeAnimationIndex);
    }
    else if (nameToUse == "ws_fadebrightnessto")
    {
      uint8_t destBrightness = valueToUse.toInt();
      Log.println("dest brightness : " + String(destBrightness));
      LedDriver._fadeDestBrightness = destBrightness;
      Log.println(String("set WS2812FX mode to ") + String(WS281xDriver::FadeBrightnessAnimationIndex));
      LedDriver.driver()->setMode(WS281xDriver::FadeBrightnessAnimationIndex);
    }
    else if (nameToUse == "ws_mode")
    {
      int val;
      int offset = parseOffset(valueToUse);
      if (offset != 0)
      {
        val = LedDriver.driver()->getMode() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }
      
      val = max(min((int)MODE_COUNT, val), (int)0);
      
      Log.println(String("set WS2812FX mode to ") + String(val));
      LedDriver.driver()->setMode(val);
    }
    else if (nameToUse == "ws_brightness")
    {
      int offset = parseOffset(valueToUse);
      int val;
      if (offset != 0)
      {
        val = LedDriver.driver()->getBrightness() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }

      val = max(min((int)BRIGHTNESS_MAX, val), (int)BRIGHTNESS_MIN);

      Log.println(String("set WS2812FX brightness to ") + String(val));
      LedDriver.driver()->setBrightness(val);
    }
    else if (nameToUse == "ws_color")
    {
      uint32_t destColor = strtol(valueToUse.c_str(), 0, 16);
      Log.println("dest color : " + String(destColor));
      LedDriver.driver()->setColor(destColor);
    }
    else if (nameToUse == "ws_speed")
    {
      int offset = parseOffset(valueToUse);
      int val;
      if (offset != 0)
      {
        val = LedDriver.driver()->getSpeed() + offset;
      }
      else
      {
        val = valueToUse.toInt();
      }

      val = max(min((int)SPEED_MAX, val), (int)SPEED_MIN);

      Log.println(String("set WS2812FX speed to ") + String(val));
      LedDriver.driver()->setSpeed(val);
    }
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
    }
    return "";
}

bool WS281xDriver::isBrightnessMin()
{
  return driver()->getBrightness() == 0;
}

bool WS281xDriver::isBrightnessMax()
{
  return driver()->getBrightness() == 255;
}

uint16_t WS281xDriver::fadeIn(void) 
{
  WS2812FX::Segment* seg = LedDriver.driver()->getSegment(); // get the current segment
  WS2812FX::Segment_runtime* segrt = LedDriver.driver()->getSegmentRuntime();

  if (segrt->counter_mode_call == 0)
  {
    Log.println("init animation");
    LedDriver._fadeSourceColor = LedDriver.driver()->getPixelColor(0);
  }

  uint16_t count = segrt->aux_param3;
  if (count < 255)
  {
    uint32_t color = LedDriver.driver()->color_blend(LedDriver._fadeSourceColor, LedDriver._fadeDestColor, count);
    for(uint16_t i = seg->start; i <= seg->stop; ++i)
    {
      LedDriver.driver()->setPixelColor(i, Adafruit_NeoPixel::gamma32(color));
    }
    segrt->aux_param3 += 1;
  }
  //else animation is finished and doesn't loop
  
  return (seg->speed / 100);
}

uint16_t WS281xDriver::fadeBrightness(void) 
{
  WS2812FX::Segment* seg = LedDriver.driver()->getSegment(); // get the current segment
  WS2812FX::Segment_runtime* segrt = LedDriver.driver()->getSegmentRuntime();

  if (segrt->counter_mode_call == 0)
  {
    LedDriver._fadeCurrentBrightness = LedDriver.driver()->getBrightness();
    if (LedDriver._fadeCurrentBrightness == 0)
    {
      //no color we have to set one
      LedDriver._colorToSet = LedDriver._onColor;
    }
    else
    {
      LedDriver._colorToSet = LedDriver.driver()->getPixelColor(0);
    }
    
  }

  for(uint16_t i = seg->start; i <= seg->stop; ++i)
  {
    LedDriver.driver()->setPixelColor(i, LedDriver._colorToSet);
  }

  if (LedDriver._fadeCurrentBrightness != LedDriver._fadeDestBrightness)
  {
    if (LedDriver._fadeCurrentBrightness > LedDriver._fadeDestBrightness)
    {
      if (LedDriver._fadeCurrentBrightness > 0)
        LedDriver._fadeCurrentBrightness--;
    }
    else
    {
      if (LedDriver._fadeCurrentBrightness < 255)
        LedDriver._fadeCurrentBrightness++;
    }
    LedDriver.driver()->setBrightness(Adafruit_NeoPixel::gamma8(LedDriver._fadeCurrentBrightness));
  }
  //else animation is finished and doesn't loop
  
  return (seg->speed / 500);
}

#if !defined(NO_GLOBAL_INSTANCES) 
WS281xDriver LedDriver;
#endif
