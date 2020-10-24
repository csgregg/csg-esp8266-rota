/**
 * @file        WiFiManager.h
 * @author      Chris Gregg
 * 
 * @brief       Manages WiFi Functions
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


#ifndef WIFI_MANAGER_H

    #define WIFI_MANAGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WiFi.h>


    enum DHCPModes { DHCP, STATIC };                // DHCP Mode


    // Sizes
    #define NET_MAX_STATIONS 3                      // Maximum number of WiFi stations that can be stored
    #define NET_MAX_SSID_LEN 32                     // Maximum length of a WiFi station SSID
    #define NET_MAX_PASSWORD_LEN 16                 // Maximum length of a WiFi station password

    // Defaults
    #define NET_DEFAULT_WIFIMODE WIFI_AP            // Options are : WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3

    #define NET_STATION_TRY_TIME 20                 // 20 sec - time to allow station to connect
    #define NET_STATION_DISCONNECT_TIME 30000       // 30 Sec - time to allow SDK to retrun before trying different station
    #define NET_STATION_SWITCH_TO_AP_TIME 60000     // 1 min - time to wait before turning on AP mode if no station connected
    #define NET_DEFAULT_DHCPMODE DHCP               // Default DHCOP mode

    #define NET_DEFAULT_AP_IP 0x0102A8C0            // Default AP IP address- 192.168.2.1
    #define NET_DEFAULT_AP_SUBNET 0x00FFFFFF        // Default AP IP subnet - 255.255.255.0
    #define NET_DEFAULT_AP_GATEWAY 0xFE02A8C0       // Default AP gateway - 192.168.2.254
    #define NET_DEFAULT_AP_CHANNEL 11               //Default AP radion channel


    /** @class StationSettings
     *  @brief Data struct class that contains the settings for a WiFi station */
    class StationSettings {

        public:

            /** Resets WiFi station settings to defaults */
            void SetDefaults();

            char SSID[NET_MAX_SSID_LEN];            // WiFi station SSID
            char password[NET_MAX_PASSWORD_LEN];    // WiFi station password
            DHCPModes DHCPMode;                     // DHCP mode
            IPAddress ip;                           // If static DHCP, IP address
            IPAddress subnet;                       // If static DHCP, IP subnet
            IPAddress gateway;                      // If static DHCP, gateway address
            IPAddress dns1;                         // If static DHCP, DNS server
            IPAddress dns2;                         // If static DHCP, DNS server

            // Create a compare operators

            bool operator== ( const StationSettings& other ) const {
                return ( strcmp( SSID, other.SSID ) == 0 )
                    && ( strcmp( password, other.password ) == 0 )
                    && DHCPMode == other.DHCPMode
                    && ip == other.ip
                    && subnet == other.subnet
                    && gateway == other.gateway
                    && dns1 == other.dns1
                    && dns2 == other.dns2;
            }
            bool operator!= ( const StationSettings& other ) const {
                return ( strcmp( SSID, other.SSID ) != 0 )
                    || ( strcmp( password, other.password ) != 0 )
                    || DHCPMode != other.DHCPMode
                    || ip != other.ip
                    || subnet != other.subnet
                    || gateway != other.gateway
                    || dns1 != other.dns1
                    || dns2 != other.dns2;
            }

    };


    /** @class APSettings
     *  @brief Data struct class that contains the settings for the WiFi AP */
    class APSettings {

        public:

            /** Resets WiFi AP settings to defaults */
            void SetDefaults();

            char SSID[NET_MAX_SSID_LEN];                // SSID for WiFi AP
            char password[NET_MAX_PASSWORD_LEN];        // Password for WiFi AP
            byte channel;                               // Radio channel used by WiFi AP
            IPAddress ip;                               // IP address for WiFi AP
            IPAddress subnet;                           // IP subnet for WiFi AP
            IPAddress gateway;                          // Gateway for WiFi AP

            // Create a compare operators

            bool operator== ( const APSettings& other ) const {
                return ( strcmp( SSID, other.SSID ) == 0 )
                    && ( strcmp( password, other.password ) == 0 )
                    && channel == other.channel
                    && ip == other.ip
                    && subnet == other.subnet
                    && gateway == other.gateway;
            }
            bool operator!= ( const APSettings& other ) const {
                return ( strcmp( SSID, other.SSID ) != 0 )
                    || ( strcmp( password, other.password ) !=0 )
                    || channel != other.channel
                    || ip != other.ip
                    || subnet != other.subnet
                    || gateway != other.gateway;
            }

    };


    /** @class WiFiSettings
     *  @brief Data struct class that contains the settings for the WiFi */
    class WiFiSettings {

        public:

            /** Resets WiFi settings to defaults */
            void SetDefaults();

            WiFiMode wifiMode;                                          // WiFi Mode        
            StationSettings stationSettings[NET_MAX_STATIONS];          // Multiple stations settings
            int lastStation = 0;                                        // Last connected station            
            APSettings apSettings;                                      // Access point mode settings

            // Create a compare operators

            bool operator== ( const WiFiSettings& other ) const {
                bool stations = true;
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = false;
                return wifiMode == other.wifiMode
                    && stations
                    && lastStation == other.lastStation
                    && apSettings == other.apSettings;
            }
            bool operator!= ( const WiFiSettings& other ) const {
                bool stations = false;
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = true;
                return wifiMode != other.wifiMode
                    && stations
                    && lastStation != other.lastStation
                    && apSettings != other.apSettings;
            }

    };


    /** @class WiFiManager
     *  @brief Manages the WiFi services */
    class WiFiManager {

        public:

            /** Constructor */
            WiFiManager()  {
                _isAPRunning = false;
                _nAPConnections = 0;
            }

            /** Initializes the WiFi Manager
             * @param stationSettings       Reference of an array of station settings
             * @param apSettings            Reference of the AP settings
             * @param wifiMode              Reference of the WiFi Mode setting */
            void ICACHE_FLASH_ATTR Begin( WiFiSettings& settings  );

            /** Handle any repeating WiFi tasks
             *  This detects if WiFi should be connected and if not, reconnects 
             *  @param force        Forces reconnect */
            void Handle( const bool force = false );

            /** Sets the WiFi Mode
             * @param mode          WiFiMode */
            void ICACHE_FLASH_ATTR SetWiFiMode( WiFiMode mode );  

            /** Reconnects all WiFi networks */
            void ICACHE_FLASH_ATTR ReconnectWifi();

            /** Connects to a specific WiFi station
             * @param id            ID of station to connect to
             * @return true         Success
             * @return false        Fail */
            bool ICACHE_FLASH_ATTR ConnectWiFiStation( const int id = 0 );

            /** Checks to see if any stored WiFi station is connected
             * @return true:        There is one
             * @return false:       None are connected */
            bool ICACHE_FLASH_ATTR IsWiFiConnected() { return WiFi.status() != WL_CONNECTED; };

            /** Checks if a specific stored station is connected - overload function
             * @param id            ID of station to check
             * @return true:        It is connected
             * @return false:       It is not connected */
            bool ICACHE_FLASH_ATTR IsStationConnected( uint id ) { return _stationConnected[id]; }

            /** Returns the currently connected WiFi station
             * @return uint         ID of connected station */
            uint ICACHE_FLASH_ATTR GetConnectedStation();

            /** Gets a reference to the reusable WiFi Client object
             *  @return WiFiClient  Reference of the client object */
            WiFiClient& ICACHE_FLASH_ATTR GetWiFiClient() { return _client; }

            /** Gets the Assigned IP address
             * @return char*        Returns IP address as a char array pointer */
            char* ICACHE_FLASH_ATTR GetAssignedIP();
          
            /** Checks to see if AP is running 
             * @return true:         Is is running
             * @return false:        It is not running */
            bool ICACHE_FLASH_ATTR IsAPRunning( ) { return _isAPRunning; }

            /** Returns the number of clients connected to the AP
             * @return uint         Number of connections */
            uint ICACHE_FLASH_ATTR CountAPConnections() { return _nAPConnections; }


        protected:
        
            /** Handles any repeating WiFi station tasks
             * @param force         Force the reconnection of the WiFi stations
             * @return true         Successfully connected
             * @return false        Failed to connect */
            bool HandleWiFiStations( const bool force = false );

            /** Handles any repeating WiFi AP tasks
             *  Including starting the AP
             * @param force         Force the restart of the AP
             * @return true         Successfully started
             * @return false        Failed to start */
            bool HandleWiFiAP( const bool force = false );

            /** Start the WiFi Access Point
             * @return true         Successfully started
             * @return false        Failed to start */
            bool ICACHE_FLASH_ATTR StartWiFiAccessPoint();

            /** Clear all station connection status */
            void ICACHE_FLASH_ATTR ResetConnectedStatus() { for( int i = 0; i < NET_MAX_STATIONS; i++ ) _stationConnected[i] = false; }

            WiFiClient _client;             // The reusable WiFi client object

            WiFiSettings* _settings;        // Pointer to the WiFi settings

            uint _disconnectedStation;                      // Used to see how long disconnected in station mode
            bool _stationConnected[NET_MAX_STATIONS];       // Array of connected statuses of the WiFi stations

            bool _isAPRunning;              // Is the AP running
            uint _nAPConnections;           // How many clients are connected

    };

#endif          // WIFI_MANAGER_H