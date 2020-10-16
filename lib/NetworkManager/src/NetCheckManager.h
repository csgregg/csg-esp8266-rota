/* Internet Check Manager Library

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

Manages Internet Checking Functions

*/


#ifndef NET_CHECK_MANAGER_H

    #define NET_CHECK_MANAGER_H

    #include <Arduino.h>
    #include <ESP8266WiFi.h>
    #include <Ticker.h>


    // Sizes
    #define NETCHECK_MAX_SERVICE_LEN 36            // Max length of generate_204 check URL
     

    class NetCheckConfig {

        public:

            void ICACHE_FLASH_ATTR setDefaults();

            bool mode;
            char checkService[NETCHECK_MAX_SERVICE_LEN];
            uint interval;

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



    class NetCheckManager {
        
        public:

            void ICACHE_FLASH_ATTR begin(WiFiClient &client, NetCheckConfig &settings);
            void handle();

            bool ICACHE_FLASH_ATTR isInternetConnected( ) { return _ConnectedToInternet; };


        protected:

            NetCheckConfig* _settings;
            WiFiClient* _client;

            bool _ConnectedToInternet;      // Is there a route to the internet
            Ticker _netCheck;
            static bool _doNetCheck;        // TODO - does this need to be static ?

            static void TriggerNetCheck() { _doNetCheck = true; };
            bool ICACHE_FLASH_ATTR NetCheck();


    };

#endif