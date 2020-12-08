#include "Logger.h"
#include "JsonConfiguration.h"

#include "WS281xDriver.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

const int WS281xDriver::FadeAnimationIndex = 55;

void WS281xDriver::setup()
{
    _ws2812fx = new WS2812FX(Configuration._numLeds, WS281X_PIN, Configuration._neoPixelType);
    _ws2812fx->init();
    _ws2812fx->setBrightness(0);
    _ws2812fx->setSpeed(1000);
    _ws2812fx->setColor(0xFFFFFF);
    _ws2812fx->setMode(FX_MODE_STATIC);
    _ws2812fx->start();
    _fadeSourceColor = 0x000000;
    _fadeDestColor = 0xFFFFFF;

    _ws2812fx->setCustomMode(F("Fade in"), fadeIn);
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
      Log.println(String("set WS2812FX mode to ") + String(WS281xDriver::FadeAnimationIndex));
      LedDriver.driver()->setMode(WS281xDriver::FadeAnimationIndex);
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

#if !defined(NO_GLOBAL_INSTANCES) 
WS281xDriver LedDriver;
#endif
