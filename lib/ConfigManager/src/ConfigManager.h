/**
 * @file        ConfigManager.h
 * @author      Chris Gregg
 * 
 * @brief       Manages saving a configuration set to the EEPROM section of the flash (First
 *              sector before file system). Settings are read and write on block and accessed
 *              through the 'settings' member. 
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

/* MIT License

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


#ifndef CONFIG_MANAGER_H

    #define CONFIG_MANAGER_H

    // Global Libraries
    #include <Arduino.h>

    // Project Libraries
    #include "NetworkManager.h"
    #include "Logger.h"
    #include "OTAUpdater.h"
    #include "TimeLocation.h"
   // #include "ThingManager.h"

    #define CONFIG_START_MARKER "CONFIG_START_23"               // Marker used to confirm presence of configs in EEPROM
    #define CONFIG_START_MARKER_SIZE 16


    /** @class DeviceSettings
     *  @brief A data structure class that contains all the settings for the device. */
    class DeviceSettings
    {

        public:

            NetworkSettings networkSettings;            // Settings for Network Manager Class
            LoggerSettings loggerSettings;              // Settings for Logger Class
            OTAUpdaterSettings otaUpdaterSettings;      // Settings for OTA Update Manager Class
            TimeLocationSettings timelocSettings;       // Settings Time and Location Manager Class

            /** Resets all the settings to the default values */
            void ICACHE_FLASH_ATTR SetDefaults();

            // Create a compare operators
            
            bool operator== ( const DeviceSettings& other ) const {
                return networkSettings == other.networkSettings
                    && loggerSettings == other.loggerSettings
                    && otaUpdaterSettings == other.otaUpdaterSettings
                    && timelocSettings == other.timelocSettings;
            }
            bool operator!= ( const DeviceSettings& other ) const {
                return networkSettings != other.networkSettings
                    || loggerSettings != other.loggerSettings
                    || otaUpdaterSettings != other.otaUpdaterSettings
                    || timelocSettings != other.timelocSettings;
            }

    };
    

    /** @class ConfigManager
     *  @brief Manages the creation, reading and writing of settings for the device. */
    class ConfigManager
    {                   

        public:

            ICACHE_FLASH_ATTR ConfigManager()
            {
                _isInitialized = false;             // Don't do anything until flash is initialized   
                static_assert( (sizeof(ConfigManager::startMarker) + sizeof(DeviceSettings)) < SPI_FLASH_SEC_SIZE, "Config too large for EEPROM sector of Flash" );
            };

            /** Start the config manager
             *  @param forceInit        Is the flash to be reset as well.
             *                          Default = false */
            void ICACHE_FLASH_ATTR Begin( const bool forceInit = false );
            void ICACHE_FLASH_ATTR SetDefaults();       /** Saves the default settings */
            void ICACHE_FLASH_ATTR Read();              /** Reads all the settings */

            /** Saves all the settings
             *  @param force    Force save if no changes, otherwise
             *                  will only be saved if different.
             *                  Default = false */
            void ICACHE_FLASH_ATTR Save( const bool force = false );
            
            DeviceSettings settings;            // Make the settings set accessible


        protected:

            /** Check for presence of marker in flash
             *  @return     true if exists */
            bool ICACHE_FLASH_ATTR CheckMarker();
            void ICACHE_FLASH_ATTR WriteMarker();       /** Write the marker to flash */
            void ICACHE_FLASH_ATTR EraseFlash();        /** Erase the flash entirely with zeros */

            bool _isInitialized;                        // Has the settings manager been initalized yet?

            // Make marker easy to refer to
            struct startMarker {
                char marker[CONFIG_START_MARKER_SIZE];
            };

    };


    extern ConfigManager config;        // Declaring the global instance

#endif      // CONFIG_MANAGER_H
