#pragma once

#include <Arduino.h>

// Version
#define VERSION                             "V1.0.0"

#define DEFAULT_HOSTNAME                    "WS21X_CONTROLLER"
#define DEFAULT_MQTTSERVERIP                "192.168.0.62"
#define DEFAULT_MQTTSERVERPORT              1883
#define DEFAULT_WS281X_LED_COUNT            90
#define DEFAULT_WS281X_NEO_PIXEL_TYPE       6 //TODO : remove
#define DEFAULT_WS281X_COLOR_LED_ON         0xFFFFFFFF

#define NTP_UPDATE_INTERVAL_MSEC     (1 * 3600 * 1000)      // Update time from NTP server every 1 hour

// LED
#define LED_PIN             LED_BUILTIN

#define MIN_TIME_FOR_SHORT_PRESS    100
#define MIN_TIME_FOR_LONG_PRESS    500
#define MAX_TIME_BETWEEN_TWO_PRESS  300
#define TIME_FOR_DIMMER_STEP        100

#define MAX_LED_COUNT               200

// ANIMATIONS

#define SPEED_MAX                   10000
#define SPEED_MIN                   200

#define ANIMATION_LUMINANCE         .5

// Timezone
#define UTC_OFFSET      +1

// change for different ntp (time servers)
#define NTP_SERVERS "0.fr.pool.ntp.org", "time.nist.gov", "pool.ntp.org"

#ifdef D1_MINI
    #define WS281X_PIN          D6

    // BUTTON
    #define BUTTON              D7
#endif

#ifdef SONOFF_BASIC
    #define WS281X_PIN          2//D6

    // BUTTON
    #define BUTTON              4//D7
#endif