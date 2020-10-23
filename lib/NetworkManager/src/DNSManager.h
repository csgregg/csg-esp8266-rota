/**
 * @file        DNSManager.h
 * @author      Chris Gregg
 * 
 * @brief       Manages DNS Functions
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


#ifndef DNS_MANAGER_H

    #define DNS_MANAGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266mDNS.h>
    #include <DNSServer.h>

    // Sizes
    #define DNS_MAX_HOSTNAME_LEN 16

    // Defaults
    #define DNS_DEFAULT_MODE true
    #define DNS_DEFAULT_MDNS true
    #define DNS_PORT 53
    #define DNS_TTL 60


    /** @class DNS settings
     *  @brief A data structure class that contains the settings for the DNS managers. */
    class DNSSettings {

        public:

            /** Resets DNS settings to defaults */
            void ICACHE_FLASH_ATTR setDefaults();

            bool dnsEnabled;                        // Is the DNS dnsEnabled
            bool mDnsEnabled;                       // Is mDNS dnsEnabled
            char hostName[DNS_MAX_HOSTNAME_LEN];    // Network name for this device

            // Create a compare operators
            
            bool operator==( const DNSSettings& other ) const {
                return ( strcmp( hostName, other.hostName) == 0 )
                    && dnsEnabled == other.dnsEnabled
                    && mDnsEnabled == other.mDnsEnabled;
            }
            bool operator!=(const DNSSettings& other) const {
                return ( strcmp(hostName, other.hostName ) != 0 )
                    || dnsEnabled != other.dnsEnabled
                    || mDnsEnabled != other.mDnsEnabled;
            }

    };


    /** @class DNS Manager
     *  @brief Manages DNS services */
    class DNSManager {
        
        public:

            /** Starts the DNS services. Only runs full DNS if in AP mode
             * @param settings      Refernce of DNS settings struct 
             * @param inApMode      Status of AP mode */
            void ICACHE_FLASH_ATTR Begin( DNSSettings& settings, bool isInApMode );

            /** Handles any repeated DNS tasks */
            void Handle();


        protected:

            DNSSettings* _settings;         // Pointer to data struct containing DNS settings
            DNSServer _dnsServer;           // Instance of DNS service
            bool _dnsStarted = false;       // Is the DNS running

    };

#endif