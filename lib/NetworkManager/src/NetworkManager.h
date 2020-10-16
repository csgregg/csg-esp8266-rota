/* Network Manager Library

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

Manages Network Functions

*/



#ifndef NETWORK_MANAGER_H

    #define NETWORK_MANAGER_H

    #include <Arduino.h>
    #include <ESP8266WiFi.h>

    #include "WiFiManager.h"
    #include "DNSManager.h"
    #include "NetCheckManager.h"

 

    class NetworkSettings {

        public:

            void ICACHE_FLASH_ATTR setDefaults();

            WiFiMode wifiMode;                                          // WiFi Mode        
            StationSettings stationSettings[NET_MAX_STATIONS];            // Multiple stations settings
            int lastStation = 0;                                        // Last connected station            
            APSettings apSettings;                                        // Access point mode settings
            NetCheckSettings netCheckSettings;                            // Connectivity checker settings
            DNSSettings dnsSettings;                                      // DNS settings
        
            // Create a compare operators
            bool operator==(const NetworkSettings& other) const {
                bool stations = true;
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = false;
                return wifiMode == other.wifiMode
                    && stations
                    && apSettings == other.apSettings
                    && netCheckSettings == other.netCheckSettings
                    && dnsSettings == other.dnsSettings;
            }
            bool operator!=(const NetworkSettings& other) const {
                bool stations = false;
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = true;
                return wifiMode != other.wifiMode
                    || stations
                    || apSettings != other.apSettings
                    || netCheckSettings != other.netCheckSettings
                    || dnsSettings != other.dnsSettings;
            }

    };



    // Network Manager Class

    class NetworkManager {

        public:

            void ICACHE_FLASH_ATTR begin( NetworkSettings &settings );
            void handle();

            void ICACHE_FLASH_ATTR reconnectWifi() { _wifi.reconnectWifi(); };
            bool ICACHE_FLASH_ATTR connectWiFiStation( const int id = 0 ) { return _wifi.connectWiFiStation( id ); };
            void ICACHE_FLASH_ATTR setWiFiMode( WiFiMode mode ) { _wifi.setWiFiMode(mode); };
            WiFiClient& ICACHE_FLASH_ATTR getWiFiClient() { return _wifi.getWiFiClient(); };
            uint ICACHE_FLASH_ATTR getConnectedStation() { return _wifi.getConnectedStation(); };
            bool ICACHE_FLASH_ATTR isStationConnected(uint id) { return _wifi.isStationConnected(id); };
            char* ICACHE_FLASH_ATTR getAssignedIP() { return _wifi.getAssignedIP(); }

            NetworkStatus ICACHE_FLASH_ATTR getNetworkStatus();

            const char* ICACHE_FLASH_ATTR getHostName() { return _settings->dnsSettings.hostname; };
            void ICACHE_FLASH_ATTR restartDNS() { _dns.begin( _settings->dnsSettings, _wifi.isAPRunning() ); };


        protected:

            NetworkSettings* _settings;     // Network settings

            // The only instances of the network services
            WiFiManager _wifi;              // WiFi service
            DNSManager _dns;                // DNS service
            NetCheckManager _netCheck;      // Net Check service

   
        private:


    };

    extern NetworkManager network;        // Global instance of the network services



#endif