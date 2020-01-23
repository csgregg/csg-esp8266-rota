/* Network Manager Library

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



#ifndef NETWORK_MANAGER_H

    #define NETWORK_MANAGER_H

    #define MAX_SSIDS 3
    #define MAX_SSID_LEN 32
    #define MAX_PASSWORD_LEN 16

    enum DHCPModes { DHCP, STATIC };         // DHCP Mode


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


    #define STATION_TRY_TIME 10                 // 20 sec - time to allow station to connect
    #define STATION_DISCONNECT_TIME 30000       // 30 Sec - time to allow SDK to retrun before trying different station
    #define STATION_SWITCH_TO_AP_TIME 60000     // 1 min - time to wait before turning on AP mode if no station connected

    // Defaults
    #define DEFAULT_WIFIMODE WIFI_STA           // Options are : WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
    #define DEFAULT_DHCPMODE STATIC
    #define DEFAULT_STATICIP 0x0101A8C0         // 192.168.1.1
    #define DEFAULT_SUBNET 0x00FFFFFF           // 255.255.255.0
    #define DEFAULT_GATEWAY 0xFE01A8C0          // 192.168.1.254
    #define DEFAULT_CHANNEL 11

 

    class NetworkSettings {

        public:

            void setWiFiDefaults();
    
            // WiFi Mode
            WiFiMode wifiMode;          

            // Save multiple networks
            StationConfig stationSettings[MAX_SSIDS];
            int lastStation = 0;

            // Access point mode settings
            APConfig apSettings;
        
            // Create a compare operators
            bool operator==(const NetworkSettings& other) const {

                bool stations = true;
                for( int i = 0; i < MAX_SSIDS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = false;

                return wifiMode == other.wifiMode
                    && stations
                    && apSettings == other.apSettings;

            }

            bool operator!=(const NetworkSettings& other) const {

                bool stations = false;
                for( int i = 0; i < MAX_SSIDS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = true;

                return wifiMode != other.wifiMode
                    || stations
                    || apSettings != other.apSettings;

            }

    };



    // Network Manager Class

    class NetworkManager {

        public:

            NetworkManager()  {
                _StationConnected = false;
                _APRunning = false;
                _APConnections = 0;
                _ConnectedToInternet = false;
            }

            void begin( NetworkSettings &settings );

            bool setWiFiStation();
            bool setWiFiAP();

            void handle();

            bool isStationConnected( ) { return _StationConnected; };
            bool isAPRunning( ) { return _APRunning; };
            bool isInternetConnected( ) { return _ConnectedToInternet; };

            WiFiClient& getWiFiClient() { return _client; };
          

        protected:

            void handleWiFi();
            bool handleWiFiStation();
            bool handleWiFiAP();

            bool startWiFiAccessPoint();
            bool startWiFiStation( const int id = 0 );
            bool checkInternet();

            void InitializeWiFi();

            NetworkSettings *_networkSettings;

            bool _StationConnected;         // Are we connected to WiFi
            bool _APRunning;                // Is the AP running
            int _APConnections;            // How many clients are connected

            bool _ConnectedToInternet;      // Is there a route to the internet
           
            int _disconnectedStation;       // Used to see how long disconnected in station mode

            WiFiClient _client;

   
        private:


    };

    extern NetworkManager network;        // Declaring the global instance



#endif