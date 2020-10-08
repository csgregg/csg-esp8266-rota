/* Remote Updater Library

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

Implements a location and time service using IPInfo.io and ezTime library.

*/



#ifndef TIME_LOCATION_H

    #define TIME_LOCATION_H

    #include <ESP8266WiFi.h>

    class Timezone;         // Forward declaration

    #define TLO_IPINFO_MAX_LOC_LEN (19+1)
    #define TLO_IPINFO_MAX_CITY_LEN (16+1)
    #define TLO_IPINFO_MAX_REGION_LEN (16+1)
    #define TLO_IPINFO_MAX_COUNTRY_LEN (2+1)
    #define TLO_IPINFO_MAX_POSTAL_LEN (8+1)
    #define TLO_IPINFO_MAX_TIMEZONE_LEN (29+1)
    #define TLO_IPINFO_MAX_TOKEN_LEN (14+1)

    #define TLO_IPINFO_RETRY (1000*60*1)    // 1 Minute

    #define TLO_MAX_LONG_DATETIME_LEN (36+1)
    

    class Location {

        public:

            struct coords {
                float lon;
                float lat;
            };

            IPAddress ip;
            char city[TLO_IPINFO_MAX_CITY_LEN] = "";
            char region[TLO_IPINFO_MAX_REGION_LEN] = "";
            char country[TLO_IPINFO_MAX_COUNTRY_LEN] = "";
            coords loc = {0,0};
            char postal[TLO_IPINFO_MAX_POSTAL_LEN] = "";
            char timezone[TLO_IPINFO_MAX_TIMEZONE_LEN] = "";

             // Create a compare operators
            bool operator==(const Location& other) const {
                return (strcmp(city,other.city)==0)
                    && (strcmp(region,other.region)==0)
                    && (strcmp(country,other.country)==0)
                    && loc.lon == other.loc.lon
                    && loc.lat == other.loc.lat
                    && (strcmp(postal,other.postal)==0)
                    && (strcmp(timezone,other.timezone)==0);
            }
            bool operator!=(const Location& other) const {
                return (strcmp(city,other.city)!=0)
                    || (strcmp(region,other.region)!=0)
                    || (strcmp(country,other.country)!=0)
                    || loc.lon != other.loc.lon
                    || loc.lat != other.loc.lat
                    || (strcmp(postal,other.postal)!=0)
                    || (strcmp(timezone,other.timezone)!=0);
            }

    };



    class TimeLocationSettings {

        public:

            void ICACHE_FLASH_ATTR setDefaults();

            bool mode = true;              // Service on or off
            bool autoLocation = true;      // Estimate location, other wise need to set manually
            char ipinfoToken[TLO_IPINFO_MAX_TOKEN_LEN] = "";
            Location location;

             // Create a compare operators
            bool operator==(const TimeLocationSettings& other) const {
                return mode == other.mode
                    && autoLocation == other.autoLocation
                    && (strcmp(ipinfoToken,other.ipinfoToken)==0)
                    && location == other.location;
            }
            bool operator!=(const TimeLocationSettings& other) const {
                return mode != other.mode
                    || autoLocation != other.autoLocation
                    || (strcmp(ipinfoToken,other.ipinfoToken)!=0)
                    || location != other.location;
            }

    };



    // Time and Location Class

    class TimeLocation {

        public:
            
            void ICACHE_FLASH_ATTR begin( TimeLocationSettings &settings );
            void ICACHE_FLASH_ATTR begin( WiFiClient& client, TimeLocationSettings &settings );

            bool ICACHE_FLASH_ATTR detectLocation();       // Use IPInfo to detect location
            void ICACHE_FLASH_ATTR saveLocation() {
                _settings->location = _location;
            };
            bool ICACHE_FLASH_ATTR updateTime();
            bool ICACHE_FLASH_ATTR isTimeSet() { return _timeStatus; };
            bool ICACHE_FLASH_ATTR isLocationSet() { return _locationStatus; };

            void ICACHE_FLASH_ATTR strcpyTimeDate(char* datetimestring);

            void handle();


        protected:

            WiFiClient* _client;
            TimeLocationSettings* _settings;
            Timezone* _timezone;
            Location _location;
            bool _timeStatus = false;
            bool _locationStatus = false;
            unsigned long _timer = 0;


        private:
        

    };



    extern TimeLocation timelocation;

#endif



