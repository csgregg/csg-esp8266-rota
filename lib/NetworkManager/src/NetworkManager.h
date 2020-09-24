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

    #include <ESP8266WiFi.h>
    #include <Ticker.h>

    #define MAX_SSIDS 3
    #define MAX_SSID_LEN 32
    #define MAX_PASSWORD_LEN 16

    enum DHCPModes { DHCP, STATIC };         // DHCP Mode

    enum NetworkStatus : uint {
        DISCONNECTED = 0,
        NORMAL = 1,                         // Connected to WiFi and internet
        NOINETERNET = 2
    };

    // Defaults
    #define STATION_TRY_TIME 10                 // 20 sec - time to allow station to connect
    #define STATION_DISCONNECT_TIME 30000       // 30 Sec - time to allow SDK to retrun before trying different station
    #define STATION_SWITCH_TO_AP_TIME 60000     // 1 min - time to wait before turning on AP mode if no station connected


    // Client network config
    class StationConfig {

        public:

            void setDefaults();

            char SSID[MAX_SSID_LEN];
            char password[MAX_PASSWORD_LEN];
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
    #define DEFAULT_WIFIMODE WIFI_AP_STA        // Options are : WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
    #define DEFAULT_DHCPMODE DHCP
    #define DEFAULT_STATICIP 0x0102A8C0         // 192.168.2.1
    #define DEFAULT_SUBNET 0x00FFFFFF           // 255.255.255.0
    #define DEFAULT_GATEWAY 0xFE02A8C0          // 192.168.2.254
    #define DEFAULT_CHANNEL 11


    // AP network config
    class APConfig {

        public:

            void setDefaults();

            char SSID[MAX_SSID_LEN];
            char password[MAX_PASSWORD_LEN];
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


    // Defaults
    #define DEFAULT_NETCHECK_INTERVAL 10        // 10 sec - how often to check connected to the internet
    #define MAX_CHECK_SERVICE_LEN 36            // Max length of generate_204 check URL
     

    class NetCheckConfig {

        public:

            void ICACHE_FLASH_ATTR setDefaults();

            bool mode = false;
            char checkService[MAX_CHECK_SERVICE_LEN] = "";      // TODO - shorten the URL
            uint interval = DEFAULT_NETCHECK_INTERVAL;

            bool operator==(const NetCheckConfig& other) const {
                return (strcmp(checkService, other.checkService)==0)
                    && mode == other.mode
                    && interval == other.interval;
            }

            bool operator!=(const NetCheckConfig& other) const {
                return (strcmp(checkService, other.checkService)!=0)
                    || mode != other.mode
                    || interval != other.interval;
            }

    };


    class NetworkSettings {

        public:

            void ICACHE_FLASH_ATTR setDefaults();
    
            // WiFi Mode
            WiFiMode wifiMode;          

            // Save multiple networks
            StationConfig stationSettings[MAX_SSIDS];
            int lastStation = 0;

            // Access point mode settings
            APConfig apSettings;

            // Connectivity checker settings
            NetCheckConfig netCheckSettings;
        
            // Create a compare operators
            bool operator==(const NetworkSettings& other) const {

                bool stations = true;
                for( int i = 0; i < MAX_SSIDS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = false;

                return wifiMode == other.wifiMode
                    && stations
                    && apSettings == other.apSettings
                    && netCheckSettings == other.netCheckSettings;

            }

            bool operator!=(const NetworkSettings& other) const {

                bool stations = false;
                for( int i = 0; i < MAX_SSIDS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = true;

                return wifiMode != other.wifiMode
                    || stations
                    || apSettings != other.apSettings
                    || netCheckSettings != other.netCheckSettings;

            }

    };



    // Network Manager Class

    class NetworkManager {

        public:

            NetworkManager()  {
                _APRunning = false;
                _APConnections = 0;
                _ConnectedToInternet = false;
            }

            void ICACHE_FLASH_ATTR begin( NetworkSettings &settings );

            bool ICACHE_FLASH_ATTR setWiFiStation();
            bool ICACHE_FLASH_ATTR setWiFiAP();
            void ICACHE_FLASH_ATTR reconnectWifi();
            bool ICACHE_FLASH_ATTR connectWiFiStation( const int id = 0 );
            void ICACHE_FLASH_ATTR setWiFiMode( WiFiMode mode ) {
                _settings->wifiMode = mode;
                handleWiFi(true);
            };            

            void handle();

            WiFiClient& ICACHE_FLASH_ATTR getWiFiClient() { return _client; };
          
            uint ICACHE_FLASH_ATTR getConnectedStation() {
                for( int i = 0; i < MAX_SSIDS; i++ ) {
                    if( _stationConnected[i] ) return i;
                }
                return 0;
            };
            bool ICACHE_FLASH_ATTR isStationConnected(uint id) { return _stationConnected[id]; };
            bool ICACHE_FLASH_ATTR isStationConnected() {
                for( int i = 0; i < MAX_SSIDS; i++ ) {
                    if( _stationConnected[i] ) return true;
                }
                return false;
            };

            bool ICACHE_FLASH_ATTR isAPRunning( ) { return _APRunning; };
            bool ICACHE_FLASH_ATTR isInternetConnected( ) { return _ConnectedToInternet; };
            NetworkStatus ICACHE_FLASH_ATTR getNetworkStatus();

            void ICACHE_FLASH_ATTR setNetChecker() { InitializeNetCheck(); }


        protected:

            void handleWiFi(const bool force = false);

            bool handleWiFiStation(const bool force = false);
            bool ICACHE_FLASH_ATTR startWiFiAccessPoint();

            bool handleWiFiAP(const bool force = false);

            void ICACHE_FLASH_ATTR InitializeWiFi();

            void ICACHE_FLASH_ATTR ResetConnectedStatus() {
                for( int i = 0; i < MAX_SSIDS; i++ ) _stationConnected[i] = false;
            };

            NetworkSettings *_settings;

            bool _APRunning;                // Is the AP running
            uint _APConnections;            // How many clients are connected

            bool _ConnectedToInternet;      // Is there a route to the internet
           
            uint _disconnectedStation;       // Used to see how long disconnected in station mode

            WiFiClient _client;

            bool _stationConnected[MAX_SSIDS];   

            Ticker _netCheck;
            static bool _doNetCheck;

            static void TriggerNetCheck();
            void ICACHE_FLASH_ATTR InitializeNetCheck();
            void HandleNetCheck();
            bool ICACHE_FLASH_ATTR NetCheck();
   
        private:


    };

    extern NetworkManager network;        // Declaring the global instance



#endif