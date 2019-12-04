/* IOT Device Library

MIT License

Copyright (c) 2019 Chris Gregg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-----------------------------------------------------------------------------

Defines the physical attributes of the IOT device and the build environment.

Build flags are loaded from platformio.ini

*/


#ifndef IOT_DEVICE_H

    #define IOT_DEVICE_H

    #include <Arduino.h>
    
    #include "Credentials.h"         // Contains private definitions (excluded from repo)
    

    // Used to stringify debug flags

    #define TEXTIFY(...) #__VA_ARGS__
    #define ESCAPEQUOTE(...) TEXTIFY(__VA_ARGS__)
    

    // In case build flags are missing for non-string members

    #ifndef UPDATE_SKIP
        #define UPDATE_SKIP 1
    #endif

    #ifndef LOG_AS_SERIAL
        #define LOG_AS_SERIAL 1
    #endif

    #ifndef LOG_AS_SERVICE
        #define LOG_AS_SERVICE 0
    #endif

    #ifndef UPDATE_INTERVAL
        #define UPDATE_INTERVAL 300
    #endif

    #ifndef LOG_LEVEL
        #define LOG_LEVEL 3
    #endif

    #ifndef MONITOR_SPEED
        #define MONITOR_SPEED 115200
    #endif


    // Turn off all debug

    #if LOG_LEVEL == 0
        #define NO_DEBUG
    #endif
   

    // Physcial board
    static const char STR_PLATFORM_P[] PROGMEM = ESCAPEQUOTE(PLATFORM);
    static const char STR_BOARD_P[] PROGMEM = ESCAPEQUOTE(BOARD);
    static const char STR_FRAMEWORK_P[] PROGMEM = ESCAPEQUOTE(FRAMEWORK);

    // General build details
    static const char STR_BUILD_TAG_P[] PROGMEM = ESCAPEQUOTE(BUILD_TAG);                       // Build tag - when used in Travis-CI comes from the GitHub Release
    static const char STR_BUILD_ENV_P[] PROGMEM = ESCAPEQUOTE(BUILD_ENV);                       // Build environmoent - Local or Travis-CI
    static const char STR_DEVICE_CODE_P[] PROGMEM = ESCAPEQUOTE(DEVICE_CODE);                   // Short code name for the device
    static const char STR_DEVICE_NAME_P[] PROGMEM = ESCAPEQUOTE(DEVICE_NAME);                   // Full device name

    // Used by CI_Remote_OTA library
    static const char STR_UPDATE_REPO_P[] PROGMEM = ESCAPEQUOTE(UPDATE_REPO);                     // GitHub reprositary holding this code
    static const char STR_UPDATE_USER_P[] PROGMEM = ESCAPEQUOTE(UPDATE_USER);                     // GitHub API user
    static const char STR_UPDATE_TOKEN_P[] PROGMEM = ESCAPEQUOTE(UPDATE_TOKEN);                   // GitHub API OAUTH token
    static const char STR_UPDATE_SERVICE_P[] PROGMEM = ESCAPEQUOTE(UPDATE_SERVICE);              // Path to PHP used to return GitHub assets
    static const bool B_SKIPUPDATES = atoi(ESCAPEQUOTE(UPDATE_SKIP));                         // Skip any updates
    static const long L_UPDATEINTERVAL = atol(ESCAPEQUOTE(UPDATE_INTERVAL));                  // Interval between update checks

    // Used by Logger library
    static const bool B_LOG_AS_SERIAL = atoi(ESCAPEQUOTE(LOG_AS_SERIAL));                     // 0 - 1 to turn on serial logging
    static const bool B_LOG_AS_SERVICE = atoi(ESCAPEQUOTE(LOG_AS_SERVICE));                   // 0 - 1 to turn on logging to Loggly service
    static const uint U_LOGGING_LEVEL = atoi(ESCAPEQUOTE(LOG_LEVEL));                         // 0 - 3 to set log level

    static const char STR_LOGGING_SERVICE_P[] PROGMEM = ESCAPEQUOTE(LOGGING_SERVICE);           // Path to Loggly API
    static const char STR_LOGGING_SERVICE_KEY_P[] PROGMEM = ESCAPEQUOTE(LOGGING_SERVICE_KEY);   // Loggly API key - stored in credentials.h for privacy
    static const char STR_LOGGING_GLOBAL_TAGS_P[] PROGMEM = ESCAPEQUOTE(LOGGING_GLOBAL_TAGS);   // Tags to globally apply to logs

    // Serial monitor speed 
    static const long L_MONITOR_BAUD = atol(ESCAPEQUOTE(MONITOR_SPEED));

#endif