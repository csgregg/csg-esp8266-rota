/* DNS Manager Library

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

Manages DNS Functions

*/


#ifndef DNS_MANAGER_H

    #define DNS_MANAGER_H

    #include <ESP8266mDNS.h>
    #include <DNSServer.h>



    #define DNS_MAX_HOSTNAME_LEN 16
    #define DNS_DEFAULT_MODE true
    #define DNS_DEFAULT_MDNS true
    #define DNS_PORT 53

    class DNSConfig {

        public:

            void ICACHE_FLASH_ATTR setDefaults();

            bool mode;
            bool mDNS;
            char hostname[DNS_MAX_HOSTNAME_LEN];

            bool operator==(const DNSConfig& other) const {
                return (strcmp(hostname, other.hostname)==0)
                    && mode == other.mode
                    && mDNS == other.mDNS;
            }

            bool operator!=(const DNSConfig& other) const {
                return (strcmp(hostname, other.hostname)!=0)
                    || mode != other.mode
                    || mDNS != other.mDNS;
            }

    };



    class DNSManager {
        
        public:

    }


#endif