/* Config Manager Library

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


*/



#ifndef CONFIG_MANAGER_H

    #define CONFIG_MANAGER_H


    #include <ESP8266WiFi.h>


    #define MAX_CONFIG_STRING_LEN 32
    #define MAX_SSIDS 3
    #define CONFIG_START_MARKER_SIZE 16
    #define CONFIG_START_MARKER "CONFIG_START_V1"


    // Make marker easy to refer to
    struct startMarker {
	    char marker[CONFIG_START_MARKER_SIZE];
    };


    // DHCP Mode
    enum dhcpMode { DHCP, STATIC };


    // Individual network config
    struct clientConfig {

        // Client mode settings
        char clientSSID[MAX_CONFIG_STRING_LEN];
        char clientPwd[MAX_CONFIG_STRING_LEN];
        dhcpMode clientDHCPMode;
        IPAddress clientStaticIP;

    };


    // Full device config set
    struct deviceSettings {

        // WiFi Mode
        WiFiMode wifiMode;          // Options are : WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3

        // Save multiple networks
        clientConfig clientSetting[MAX_SSIDS];

        // Access point mode settings
        char apSSID[MAX_CONFIG_STRING_LEN];
        char apPwd[MAX_CONFIG_STRING_LEN];
        byte apChannel;

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

            void ResetToDefaults();
            void ReadConfig();
            void SaveConfig();

            deviceSettings Settings;


        protected:

            bool CheckFlashInitialized();
            void InitializeFlash();
            void EraseFlash();

            bool _IsInitialized;

        //    startMarker _markerData;


    };

    extern ConfigManager config;


#endif
