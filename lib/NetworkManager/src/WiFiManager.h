/* WiFi Manager Library

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

Manages WiFi Functions

*/



#ifndef WIFI_MANAGER_H

    #define WIFI_MANAGER_H

    #include <Arduino.h>
    #include <ESP8266WiFi.h>


    #define NET_MAX_STATIONS 3
    #define NET_MAX_SSID_LEN 32
    #define NET_MAX_PASSWORD_LEN 16


    enum DHCPModes { DHCP, STATIC };         // DHCP Mode

    enum NetworkStatus : uint {
        DISCONNECTED = 0,
        NORMAL = 1,                         // Connected to WiFi and internet
        NOINETERNET = 2
    };


    // Defaults
    #define NET_STATION_TRY_TIME 20                 // 20 sec - time to allow station to connect
    #define NET_STATION_DISCONNECT_TIME 30000       // 30 Sec - time to allow SDK to retrun before trying different station
    #define NET_STATION_SWITCH_TO_AP_TIME 60000     // 1 min - time to wait before turning on AP mode if no station connected


    // Client network config
    class StationConfig {

        public:

            void setDefaults();

            char SSID[NET_MAX_SSID_LEN];
            char password[NET_MAX_PASSWORD_LEN];
            DHCPModes DHCPMode;
            IPAddress ip;
            IPAddress subnet;
            IPAddress gateway;
            IPAddress dns1;
            IPAddress dns2;

            // Create a compare operators

            bool operator==(const StationConfig& other) const {
                return (strcmp(SSID, other.SSID)==0)
                    && (strcmp(password, other.password)==0)
                    && DHCPMode == other.DHCPMode
                    && ip == other.ip
                    && subnet == other.subnet
                    && gateway == other.gateway
                    && dns1 == other.dns1
                    && dns2 == other.dns2;
            }

            bool operator!=(const StationConfig& other) const {
                return (strcmp(SSID, other.SSID)!=0)
                    || (strcmp(password, other.password)!=0)
                    || DHCPMode != other.DHCPMode
                    || ip != other.ip
                    || subnet != other.subnet
                    || gateway != other.gateway
                    || dns1 != other.dns1
                    || dns2 != other.dns2;
            }

    };


    // Defaults
    #define NET_DEFAULT_WIFIMODE WIFI_AP            // Options are : WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
    #define NET_DEFAULT_DHCPMODE DHCP
    #define NET_DEFAULT_STATICIP 0x0102A8C0         // 192.168.2.1
    #define NET_DEFAULT_SUBNET 0x00FFFFFF           // 255.255.255.0
    #define NET_DEFAULT_GATEWAY 0xFE02A8C0          // 192.168.2.254
    #define NET_DEFAULT_CHANNEL 11


    // AP network config
    class APConfig {

        public:

            void setDefaults();

            char SSID[NET_MAX_SSID_LEN];
            char password[NET_MAX_PASSWORD_LEN];
            byte channel;
            IPAddress ip;
            IPAddress subnet;
            IPAddress gateway;

            // Create a compare operators

            bool operator==(const APConfig& other) const {
                return (strcmp(SSID, other.SSID)==0)
                    && (strcmp(password, other.password)==0)
                    && channel == other.channel
                    && ip == other.ip
                    && subnet == other.subnet
                    && gateway == other.gateway;
            }
            bool operator!=(const APConfig& other) const {
                return (strcmp(SSID, other.SSID)!=0)
                    || (strcmp(password, other.password)!=0)
                    || channel != other.channel
                    || ip != other.ip
                    || subnet != other.subnet
                    || gateway != other.gateway;
            }

    };




    // WiFi Manager Class

    class WiFiManager {

        public:

            WiFiManager()  {
                _APRunning = false;
                _APConnections = 0;
            }

            void ICACHE_FLASH_ATTR begin( StationConfig* const &stationSettings, APConfig &apSettings, WiFiMode &wifiMode );            // TODO - const this elsewhere?

            WiFiClient* ICACHE_FLASH_ATTR getClient() { return &_client; };

            bool ICACHE_FLASH_ATTR setWiFiStation();
            bool ICACHE_FLASH_ATTR setWiFiAP();
            void ICACHE_FLASH_ATTR reconnectWifi();
            bool ICACHE_FLASH_ATTR connectWiFiStation( const int id = 0 );
            void ICACHE_FLASH_ATTR setWiFiMode( WiFiMode mode ) {
                *_wifiMode = mode;
                handle(true);
            };            

            void handle(const bool force = false);

            WiFiClient& ICACHE_FLASH_ATTR getWiFiClient() { return _client; };
          
            uint ICACHE_FLASH_ATTR getConnectedStation() {
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) {
                    if( _stationConnected[i] ) return i;
                }
                return 0;
            };
            uint ICACHE_FLASH_ATTR countAPConnections() { return _APConnections; };
            bool ICACHE_FLASH_ATTR isStationConnected(uint id) { return _stationConnected[id]; };
            bool ICACHE_FLASH_ATTR isStationConnected() {
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) {
                    if( _stationConnected[i] ) return true;
                }
                return false;
            };

            bool ICACHE_FLASH_ATTR isAPRunning( ) { return _APRunning; };
            NetworkStatus ICACHE_FLASH_ATTR getNetworkStatus();

            char* ICACHE_FLASH_ATTR getAssignedIP() {
                static char ip[16];
                strcpy(ip, WiFi.localIP().toString().c_str());
                return ip;
            }


        protected:

            void handleWiFi(const bool force = false);
            bool handleWiFiAP(const bool force = false);
            bool handleWiFiStation(const bool force = false);

            bool ICACHE_FLASH_ATTR startWiFiAccessPoint();
            void ICACHE_FLASH_ATTR StartWiFi();

            void ICACHE_FLASH_ATTR ResetConnectedStatus() {
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) _stationConnected[i] = false;
            };

            WiFiClient _client;

            WiFiMode* _wifiMode;              // WiFi Mode

            StationConfig* const * _stationSettings;
            int _lastStation = 0;
            uint _disconnectedStation;       // Used to see how long disconnected in station mode
            bool _stationConnected[NET_MAX_STATIONS];   

            APConfig* _apSettings;
            bool _APRunning;                // Is the AP running
            uint _APConnections;            // How many clients are connected
   

        private:


    };

#endif