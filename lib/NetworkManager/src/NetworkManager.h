/**
 * @file        NetworkManager.h
 * @author      Chris Gregg
 * 
 * @brief       Manages Network Services
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


#ifndef NETWORK_MANAGER_H

    #define NETWORK_MANAGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WiFi.h>

    // Project Libraries
    #include "WiFiManager.h"
    #include "DNSManager.h"
    #include "NetCheckManager.h"

 
    /** @class Network settings
     *  @brief A data structure class that contains the network settings. */
    class NetworkSettings {

        public:

            /** Resets logger settings to defaults */
            void ICACHE_FLASH_ATTR SetDefaults();

            WiFiMode wifiMode;                                          // WiFi Mode        
            StationSettings stationSettings[NET_MAX_STATIONS];          // Multiple stations settings
            int lastStation = 0;                                        // Last connected station            
            APSettings apSettings;                                      // Access point mode settings
            NetCheckSettings netCheckSettings;                          // Connectivity checker settings
            DNSSettings dnsSettings;                                    // DNS settings
        
            // Create a compare operators
            
            bool operator== ( const NetworkSettings& other ) const {
                bool stations = true;
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = false;
                return wifiMode == other.wifiMode
                    && stations
                    && apSettings == other.apSettings
                    && netCheckSettings == other.netCheckSettings
                    && dnsSettings == other.dnsSettings;
            }
            bool operator!= ( const NetworkSettings& other ) const {
                bool stations = false;
                for( int i = 0; i < NET_MAX_STATIONS; i++ ) if( stationSettings[i] != other.stationSettings[i] ) stations = true;
                return wifiMode != other.wifiMode
                    || stations
                    || apSettings != other.apSettings
                    || netCheckSettings != other.netCheckSettings
                    || dnsSettings != other.dnsSettings;
            }

    };


    /** @class Network Manager
     *  @brief Manages all the network services */
    class NetworkManager {

        public:

            // Status of the network connection
            enum NetworkStatus : uint {
                DISCONNECTED = 0,                   // No network connection
                NORMAL = 1,                         // Connected to WiFi and internet
                NOINETERNET = 2                     // Connected to WiFi but no internet
            };

            /** Start network services
             * @param settings      Reference to struct containing network settings */
            void ICACHE_FLASH_ATTR Begin( NetworkSettings& settings );

            /** Handles any repeated nertwork tasks */
            void Handle();

            // WiFi wrappers

            /** Reconnects the WiFi */
            void ICACHE_FLASH_ATTR ReconnectWifi() { _wifi.ReconnectWifi(); }

            /** Connects to a specific WiFi station
             *  @param id    ID of station, 0 to NET_MAX_STATIONS
             *  @return true:    Successfully connected
             *  @return false    Unsuccessfully connected */
            bool ICACHE_FLASH_ATTR ConnectWiFiStation( const int id = 0 ) { return _wifi.ConnectWiFiStation( id ); }

            /** Sets the WiFi mode
             *  @param mode     WiFiMode to set */
            void ICACHE_FLASH_ATTR SetWiFiMode( WiFiMode mode ) { _wifi.SetWiFiMode(mode); }

            /** Get the WiFi Client object
             * @return WiFiClient   Reference of reusable WiFiClient */
            WiFiClient& ICACHE_FLASH_ATTR GetWiFiClient() { return _wifi.GetWiFiClient(); }

            /** Gets the ID of the connected WiFi station
             *  @return uint    ID of the connected station */
            uint ICACHE_FLASH_ATTR GetConnectedStation() { return _wifi.GetConnectedStation(); }

            /** Check if a WiFi station is connected
             *  @param id       ID of station to check
             *  @return true:   The station is connected
             *  @return false:  The station is not connected */
            bool ICACHE_FLASH_ATTR IsStationConnected( uint id ) { return _wifi.IsStationConnected(id); }

            /** Get the IP address assigned by the connected WiFi station
             *  @return     Pointer to char array of IP address */
            char* ICACHE_FLASH_ATTR GetAssignedIP() { return _wifi.GetAssignedIP(); }

            /** Gets the current status of the network
             *  @return NetworkStatus         Status of network */
            NetworkStatus ICACHE_FLASH_ATTR GetNetworkStatus();

            /** Gets the host name for the device on the network
             *  @return char*   Host name */
            const char* ICACHE_FLASH_ATTR GetHostName() { return _settings->dnsSettings.hostName; }

            /** Restarts the DNS services */
            void ICACHE_FLASH_ATTR RestartDNS() { _dns.Begin( _settings->dnsSettings, _wifi.IsAPRunning() ); };


        protected:

            NetworkSettings* _settings;     // Pointer to data struct for the network settings

            // The only instances of the network services
            WiFiManager _wifi;              // WiFi service
            DNSManager _dns;                // DNS service
            NetCheckManager _netCheck;      // Net Check service

    };


    extern NetworkManager network;        // Global instance of the network services

#endif              // NETWORK_MANAGER_H