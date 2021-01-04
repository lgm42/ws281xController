#pragma once

#include <Arduino.h>
#include "WS2812FX.h"

// Version
#define VERSION                             "V1.0.0"

#define DEFAULT_HOSTNAME                    "WS21X_CONTROLLER"
#define DEFAULT_MQTTIPSERVER                "192.168.0.62"
#define DEFAULT_MQTTPORTSERVER              1883
#define DEFAULT_WS281X_LED_COUNT            90
#define DEFAULT_WS281X_NEO_PIXEL_TYPE       NEO_RGB + NEO_KHZ800
#define DEFAULT_WS281X_COLOR_LED_ON         0xFFFFFFFF

#define NTP_UPDATE_INTERVAL_MSEC     (1 * 3600 * 1000)      // Update time from NTP server every 1 hour

// LED
#define LED_PIN             LED_BUILTIN
#define WS281X_PIN          2//D6

// BUTTON
#define BUTTON              4//D7

#define MIN_TIME_FOR_SHORT_PRESS    100
#define MIN_TIME_FOR_LONG_PRESS    500
#define MAX_TIME_BETWEEN_TWO_PRESS  300
#define TIME_FOR_DIMMER_STEP        100

// Timezone
#define UTC_OFFSET      +1

// change for different ntp (time servers)
#define NTP_SERVERS "0.fr.pool.ntp.org", "time.nist.gov", "pool.ntp.org"