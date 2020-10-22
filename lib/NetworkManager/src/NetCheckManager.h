/**
 * @file        NetCheckManager.h
 * @author      Chris Gregg
 * 
 * @brief       Manages Internet Checking Functions
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


#ifndef NET_CHECK_MANAGER_H

    #define NET_CHECK_MANAGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WiFi.h>
    #include <Ticker.h>


    // Sizes
    #define NETCHECK_MAX_SERVICE_LEN 36            // Max length of generate_204 check URL
     

    /** @class NetCheckSettings
     *  @brief Data struct class containings the settings for the internet connectivity checker */
    class NetCheckSettings {

        public:

            /** Resets internet checker settings to defaults */
            void ICACHE_FLASH_ATTR setDefaults();

            bool isOn;                                          // Is the checking service on
            char checkService[NETCHECK_MAX_SERVICE_LEN];        // URL of the check destination
            uint interval;                                      // Interval between checks (sec)

            bool operator==(const NetCheckSettings& other) const {
                return (strcmp(checkService, other.checkService)==0)
                    && isOn == other.isOn
                    && interval == other.interval;
            }
            bool operator!=(const NetCheckSettings& other) const {
                return (strcmp(checkService, other.checkService)!=0)
                    || isOn != other.isOn
                    || interval != other.interval;
            }

    };


    /** @class  NetCheckManager
     *  @brief  Checks for connectivity to the internet
     *          Confirms access to a specific file on a server */
    class NetCheckManager {
        
        public:

            /** Starts the net check service
             * @param client        // Reference of a reusable WiFiClient
             * @param settings      // Reference of the settings struct */
            void ICACHE_FLASH_ATTR Begin( WiFiClient& client, NetCheckSettings& settings );

            /** Handle repeated net check tasks */
            void Handle();

            /** Checks status of internet connectivity flag
             * @return true:         Connected to internet
             * @return false:        Not connected */
            bool ICACHE_FLASH_ATTR isInternetConnected( ) { return _isConnectedToInternet; }


        protected:

            /** Function called by Ticker interrupt */
            static void TriggerNetCheck() { _doNetCheck = true; };

            /** Perform internet check */
            bool ICACHE_FLASH_ATTR NetCheck();

            NetCheckSettings* _settings;    // Pointer to the settings struct
            WiFiClient* _client;            // Pointer to a reuable WiFiClient

            bool _isConnectedToInternet;    // Is there a route to the internet
            Ticker _netCheck;               // Ticker object to trigger check events
            static bool _doNetCheck;        // Flag that indicates time to run check            // TODO - does this need to be static ?

    };

#endif