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
    

    // In case build flags are missing for non-string flags

    #ifndef UPDATER_SKIP
        #define UPDATER_SKIP 1
    #endif

    #ifndef LOGGER_AS_SERIAL
        #define LOGGER_AS_SERIAL 1
    #endif

    #ifndef LOGGER_AS_SERVICE
        #define LOGGER_AS_SERVICE 0
    #endif

    #ifndef UPDATER_INTERVAL
        #define UPDATER_INTERVAL 300
    #endif

    #ifndef LOGGER_LEVEL
        #define LOGGER_LEVEL 3
    #endif

    #ifndef MONITOR_SPEED
        #define MONITOR_SPEED 115200
    #endif


    // Turn off all debug

    #if LOGGER_LEVEL == 0
        #define NO_DEBUG
    #endif
   
    // Physcial board
    static const char flag_PLATFORM [] PROGMEM = ESCAPEQUOTE(PLATFORM);                         // Device platform
    static const char flag_BOARD [] PROGMEM = ESCAPEQUOTE(BOARD);                               // Specific board
    static const char flag_FRAMEWORK [] PROGMEM = ESCAPEQUOTE(FRAMEWORK);                       // Device framwork

    // General build details
    static const char flag_BUILD_TAG [] PROGMEM = ESCAPEQUOTE(BUILD_TAG);                       // Build tag - when used in Travis-CI comes from the GitHub Release
    static const char flag_BUILD_ENV [] PROGMEM = ESCAPEQUOTE(BUILD_ENV);                       // Build environmoent - Local or Travis-CI
    static const char flag_DEVICE_CODE [] PROGMEM = ESCAPEQUOTE(DEVICE_CODE);                   // Short code name for the device
    static const char flag_DEVICE_NAME [] PROGMEM = ESCAPEQUOTE(DEVICE_NAME);                   // Full device name

    // Used by CI_Remote_OTA library
    static const char flag_UPDATER_REPO [] PROGMEM = ESCAPEQUOTE(UPDATER_REPO);                 // GitHub reprositary holding this code
    static const char flag_UPDATER_USER [] PROGMEM = ESCAPEQUOTE(UPDATER_USER);                 // GitHub API user
    static const char flag_UPDATER_TOKEN [] PROGMEM = ESCAPEQUOTE(UPDATER_TOKEN);               // GitHub API OAUTH token
    static const char flag_UPDATER_SERVICE [] PROGMEM = ESCAPEQUOTE(UPDATER_SERVICE);           // Path to PHP used to return GitHub assets
    static const bool flag_UPDATER_SKIP = atoi(ESCAPEQUOTE(UPDATER_SKIP));                      // Skip any updates
    static const long flag_UPDATER_INTERVAL = atol(ESCAPEQUOTE(UPDATER_INTERVAL));              // Interval between update checks


    // Used by Logger library
    static const bool flag_LOGGVER_AS_SERIAL = atoi(ESCAPEQUOTE(LOGGER_AS_SERIAL));             // 0 - 1 to turn on serial logging
    static const bool flag_LOGGER_AS_SERVICE = atoi(ESCAPEQUOTE(LOGGER_AS_SERVICE));            // 0 - 1 to turn on logging to Loggly service
    static const uint flag_LOGGER_LEVEL = atoi(ESCAPEQUOTE(LOGGER_LEVEL));                      // 0 - 3 to set log level
    static const char flag_LOGGER_SERVICE [] PROGMEM = ESCAPEQUOTE(LOGGER_SERVICE);             // Path to Loggly API
    static const char flag_LOGGER_SERVICE_KEY [] PROGMEM = ESCAPEQUOTE(LOGGER_SERVICE_KEY);     // Loggly API key - stored in credentials.h for privacy
    static const char flag_LOGGER_GLOBAL_TAGS [] PROGMEM = ESCAPEQUOTE(LOGGER_GLOBAL_TAGS);     // Tags to globally apply to logs
    static const long flag_MONITOR_SPEED = atol(ESCAPEQUOTE(MONITOR_SPEED));                    // Monitor baud


    // Expand the EspClass to add build flags, etc
    class IOTDevice : public EspClass {
        public:

            // Get build flags + overloads
            String getBuildFlag( const char * name, const char * flag, bool described = false );
            String getBuildFlag( const char * name, const bool flag, bool decribed );
            bool getBuildFlag( const char * name, const bool flag );
            String getBuildFlag( const char * name, const uint flag, bool decribed );
            uint getBuildFlag( const char * name, const uint flag );
            String getBuildFlag( const char * name, const long flag, bool decribed );
            long getBuildFlag( const char * name, const long flag );

            // Get unique chip ID
            String getChipId(bool described);

    };

    extern IOTDevice device;

    #define device_getBuildFlag(name, ...) device.getBuildFlag(#name, name, ##__VA_ARGS__)

#endif