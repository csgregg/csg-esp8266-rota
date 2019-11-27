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

    #ifndef SKIP_UPDATES
        #define SKIP_UPDATES 1
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


    // IOT Device Class

    class IOTDevice {

        public:

            // Physcial board
            
            const String platform = ESCAPEQUOTE(PLATFORM);
            const String board = ESCAPEQUOTE(BOARD);
            const String framework = ESCAPEQUOTE(FRAMEWORK);
           
           // General build details

            const String buildTag = ESCAPEQUOTE(BUILD_TAG);                         // Build tag - when used in Travis-CI comes from the GitHub Release
            const String deviceCode = ESCAPEQUOTE(DEVICE_CODE);                     // Short code name for the device
            const String deviceName = ESCAPEQUOTE(DEVICE_NAME);                     // Full device name

            // Used by CI_Remote_OTA library

            const String repoName = ESCAPEQUOTE(DEVICE_REPO);                       // GitHub reprositary holding this code
            const String assetService = ESCAPEQUOTE(ASSET_SERVICE);                 // Path to PHP used to return GitHub assets
            const bool skipUpdates = atoi(ESCAPEQUOTE(SKIP_UPDATES));               // Skip any updates
            const float updateInterval = atof(ESCAPEQUOTE(UPDATE_INTERVAL));        // Interval between update checks

            // Used by Logger library

            const bool logAsSerial = atoi(ESCAPEQUOTE(LOG_AS_SERIAL));              // 0 - 1 to turn on serial logging
            const bool logAsService = atoi(ESCAPEQUOTE(LOG_AS_SERVICE));            // 0 - 1 to turn on logging to Loggly service
            const uint loggingLevel = atoi(ESCAPEQUOTE(LOG_LEVEL));                 // 0 - 3 to set log level

            const String loggingService = ESCAPEQUOTE(LOGGING_SERVICE);             // Path to Loggly API
            const String loggingServiceKey = ESCAPEQUOTE(LOGGING_SERVICE_KEY);      // Loggly API key - stored in credentials.h for privacy
            const String loggingGlobalTags = ESCAPEQUOTE(LOGGING_GLOBAL_TAGS);      // Tags to globally apply to logs

            // Serial monitor speed 
            
            const long monitorBaud = atol(ESCAPEQUOTE(MONITOR_SPEED));


        protected:


        private:


    };

    extern IOTDevice device;              // Declaring global instance

#endif