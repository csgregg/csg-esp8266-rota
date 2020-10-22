/* MIT License

Copyright (c) 2020 Chris Gregg

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
SOFTWARE. */

/** Defines the physical attributes of the IOT device and the build environment.
 *  Build flags are loaded from platformio.ini
 * 
 *  @file   IOTDevice.h
 *  @author Chris Gregg
 *  @date   2020
 *  @version
 */


#ifndef IOT_DEVICE_H

    #define IOT_DEVICE_H

    // Global Libraries
    #include <Arduino.h>
    #include <DoubleResetDetector.h>
    
    // Project Libraries
    #include "Credentials.h"                // Contains private definitions (excluded from repo)

    #ifndef BUILD_NUMBER
        #include "Version.h"                // Defines local build number
    #endif


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

    #ifndef LOGGER_TICK_INTERVAL
        #define LOGGER_TICK_INTERVAL 60
    #endif

    #ifndef LOGGER_TICKER
        #define LOGGER_TICKER 0
    #endif

    #ifndef LOGGER_LEVEL
        #define LOGGER_LEVEL 3
    #endif

    #ifndef MONITOR_SPEED
        #define MONITOR_SPEED 115200
    #endif

    #ifndef BUILD_NUMBER
        #define BUILD_NUMBER 0
    #endif

    #ifndef BUILD_ENV
        #define BUILD_ENV None
    #endif

    #ifndef BUILD_TIMESTAMP
        #define BUILD_TIMESTAMP None
    #endif


    // Sizes
    #define FLAG_MAX_BUILD_NO_LEN (5+1)
    #define FLAG_MAX_BUILD_TIMESTAMP_LEN (24+1)
    #define FLAG_MAX_CHIPIN_LEN (8+1)
    #define FLAG_MAX_BUILDENV_LEN (32+1)

   // Detect double reset
    #define DRD_TIMEOUT 3           // Number of seconds after reset during which a subseqent reset will be considered a double reset.
    #define DRD_ADDRESS 0           // RTC Memory Address for the DoubleResetDetector to use


    // Physical board
    static const char flag_BOARD [] PROGMEM = ESCAPEQUOTE(BOARD);                               // Specific board

    // General build details
    static const char flag_BUILD_TAG [] PROGMEM = ESCAPEQUOTE(BUILD_TAG);                       // Build tag - when used in Travis-CI comes from the GitHub Release
    static const char flag_BUILD_ENV [] PROGMEM = ESCAPEQUOTE(BUILD_ENV);                       // Build environmoent - Local or Travis-CI
    static const char flag_DEVICE_CODE [] PROGMEM = ESCAPEQUOTE(DEVICE_CODE);                   // Short code name for the device
    static const char flag_DEVICE_NAME [] PROGMEM = ESCAPEQUOTE(DEVICE_NAME);                   // Full device name
    static const uint flag_BUILD_NO = atoi(ESCAPEQUOTE(BUILD_NUMBER));                          // Get build number
    static const char flag_BUILD_TIMESTAMP [] PROGMEM = ESCAPEQUOTE(BUILD_TIMESTAMP);           // Set build date and time
    
    // Used by Netork Manager
    static const bool flag_NET_CHECKER = atoi(ESCAPEQUOTE(NET_CHECKER));                        // 0 - 1 to turn on internet connectivity checker
    static const char flag_NET_CHECK_SERVICE [] PROGMEM = ESCAPEQUOTE(NET_CHECK_SERVICE);       // Generate 204 script
    static const uint flag_NET_CHECK_INTERVAL = atoi(ESCAPEQUOTE(NET_CHECK_INTERVAL));          // Interval between checks for internet connectivity

    // Used by Remote OTA library
    static const char flag_UPDATER_REPO [] PROGMEM = ESCAPEQUOTE(UPDATER_REPO);                 // GitHub reprositary holding this code
    static const char flag_UPDATER_USER [] PROGMEM = ESCAPEQUOTE(UPDATER_USER);                 // GitHub API user
    static const char flag_UPDATER_TOKEN [] PROGMEM = ESCAPEQUOTE(UPDATER_TOKEN);               // GitHub API OAUTH token
    static const char flag_UPDATER_SERVICE [] PROGMEM = ESCAPEQUOTE(UPDATER_SERVICE);           // Path to PHP used to return GitHub assets
    static const bool flag_UPDATER_SKIP = atoi(ESCAPEQUOTE(UPDATER_SKIP));                      // Skip any updates
    static const uint flag_UPDATER_INTERVAL = atoi(ESCAPEQUOTE(UPDATER_INTERVAL));              // Interval between update checks (sec)

    // Used by Logger library
    static const bool flag_LOGGER_AS_SERIAL = atoi(ESCAPEQUOTE(LOGGER_AS_SERIAL));              // 0 - 1 to turn on serial logging
    static const bool flag_LOGGER_AS_SERVICE = atoi(ESCAPEQUOTE(LOGGER_AS_SERVICE));            // 0 - 1 to turn on logging to Loggly service
    static const bool flag_LOGGER_TICKER = atoi(ESCAPEQUOTE(LOGGER_TICKER));                    // 0 - 1 to turn on ticking to Loggly service
    static const uint flag_LOGGER_TICK_INTERVAL = atoi(ESCAPEQUOTE(LOGGER_TICK_INTERVAL));      // Internal between ticks (sec)
    static const uint flag_LOGGER_LEVEL = atoi(ESCAPEQUOTE(LOGGER_LEVEL));                      // 0 - 4 to set log level
    static const char flag_LOGGER_SERVICE [] PROGMEM = ESCAPEQUOTE(LOGGER_SERVICE);             // Path to Loggly API
    static const char flag_LOGGER_SERVICE_KEY [] PROGMEM = ESCAPEQUOTE(LOGGER_SERVICE_KEY);     // Loggly API key - stored in credentials.h for privacy
    static const char flag_LOGGER_GLOBAL_TAGS [] PROGMEM = ESCAPEQUOTE(LOGGER_GLOBAL_TAGS);     // Tags to globally apply to logs
    static const uint flag_MONITOR_SPEED = atoi(ESCAPEQUOTE(MONITOR_SPEED));                    // Monitor baud

    // Used by Time / Location Library
    static const char flag_TLO_IPINFO_TOKEN [] PROGMEM = ESCAPEQUOTE(TLO_IPINFO_TOKEN);         // Token for IPInfo.io service
    static const char flag_TLO_IPINFO_SERVICE [] PROGMEM = ESCAPEQUOTE(TLO_IPINFO_SERVICE);     // URL for IPInfo.io service


    /** @class IOT Device Class
     * 
     *  @brief Expand the EspClass to add build flags, and contains any hardware specifics. */
    class IOTDevice : public EspClass
    {

        public:

            enum StartMode {
                NORMAL,
                DOUBLERESET,
            };

            /** Constructor */
            IOTDevice() : 
                _drd( DRD_TIMEOUT, DRD_ADDRESS )        // Set up double reset detection
            {};

            /** Sets up the device hardware and build environment */
            void ICACHE_FLASH_ATTR Begin();

            /** Get the build number
             *  @returns String containing the number of the build */
            char* ICACHE_FLASH_ATTR GetBuildNo() { return _buildNo; };

            /** Get the build time stamp
             *  @returns String containing the date and time of the build */
            char* ICACHE_FLASH_ATTR GetBuildTime() { return _buildTime; };

            /** Get the chip ID
             *  @returns String containing the chip ID */            
            char* ICACHE_FLASH_ATTR GetChipId() { return _chipID; };

            /** Get the build environment
             *  @returns String containing the build environment */     
            char* ICACHE_FLASH_ATTR GetBuildEnv() { return _buildEnv; };

            /** Get the mode the device was started in
             *  @returns StartMode */         
            StartMode ICACHE_FLASH_ATTR GetStartMode() { return _startMode; };

            /** Handles any repeating device actions */    
            void Handle() { _drd.loop(); };


        protected:

            char _buildNo[FLAG_MAX_BUILD_NO_LEN];               // Stores the build number as char array                // TODO - this is double storage. Must be a better way.
            char _buildTime[FLAG_MAX_BUILD_TIMESTAMP_LEN];      // Stores the build time stamp as char array
            char _chipID[FLAG_MAX_CHIPIN_LEN];                  // Stores the chip ID as char array
            char _buildEnv[FLAG_MAX_BUILDENV_LEN];              // Stores build environment as char array

            DoubleResetDetector _drd;                           // Creating the double reset detector
            StartMode _startMode = NORMAL;                      // The mode the device was started in. Defaults to NORMAL
    };


    extern IOTDevice device;        // Declaring the global instance

#endif      // IOT_DEVICE_H
