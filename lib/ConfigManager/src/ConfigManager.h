/* Config Manager Library

MIT License

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
SOFTWARE.

-----------------------------------------------------------------------------

Manages saving a configuration set to the EEPROM section of the flash (First
sector before file system). Settings are read and write on block and accessed
through the settings member.

*/



#ifndef CONFIG_MANAGER_H

    #define CONFIG_MANAGER_H

    #include <Arduino.h>

    #include "NetworkManager.h"
    #include "Logger.h"
    #include "OTAUpdater.h"
    #include "TimeLocation.h"


    #define MAX_CONFIG_STRING_LEN 32
    #define CONFIG_START_MARKER_SIZE 16
    #define CONFIG_START_MARKER "CONFIG_START_11"


    // Make marker easy to refer to
    struct startMarker {
	    char marker[CONFIG_START_MARKER_SIZE];
    };


    // Configuration Definitions

    struct deviceSettings
    {
        NetworkSettings networkConfig;
        LogSettings logConfig;
        OTASettings otaConfig;
        TimeLocationSettings timelocConfig;

        // Create a compare operators
        bool operator==(const deviceSettings& other) const {
            return networkConfig == other.networkConfig
                && logConfig == other.logConfig
                && otaConfig == other.otaConfig
                && timelocConfig == other.timelocConfig;
        }

        bool operator!=(const deviceSettings& other) const {
            return networkConfig != other.networkConfig
                || logConfig != other.logConfig
                || otaConfig != other.otaConfig
                || timelocConfig != other.timelocConfig;
        }

    };
    

    // How big is everything
    const size_t markerDataSize = sizeof(startMarker);
    const size_t settingsDataSize = sizeof(deviceSettings);
    const size_t configFlashSize = (markerDataSize + settingsDataSize);

    static_assert( configFlashSize < SPI_FLASH_SEC_SIZE, "Config too large for EEPROM sector of Flash");


    // Configuration Manager Class

    class ConfigManager {

        public:

            ConfigManager();

            void ICACHE_FLASH_ATTR begin(const bool forceInit = false);          // Start the config manager
            void ICACHE_FLASH_ATTR ResetToDefaults();                                 // Saves the default settings
            void ICACHE_FLASH_ATTR Read();                                            // Reads all the settings
            void ICACHE_FLASH_ATTR Save(const bool force = false);                    // Saves all the settings

            // Make the settings set accessible
            deviceSettings settings;


        protected:

            bool ICACHE_FLASH_ATTR CheckMarker();
            void ICACHE_FLASH_ATTR WriteMarker();
            void ICACHE_FLASH_ATTR EraseFlash();

            bool _IsInitialized;

        private:


    };

    extern ConfigManager config;        // Declaring the global instance

#endif
