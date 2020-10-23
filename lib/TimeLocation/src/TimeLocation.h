/**
 * @file        TimeLocation.h
 * @author      Chris Gregg
 * 
 * @brief       Implements a location and time service using IPInfo.io and ezTime library.
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


#ifndef TIME_LOCATION_H

    #define TIME_LOCATION_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WiFi.h>


    // Sizes
    #define TLO_IPINFO_MAX_LOC_LEN (19+1)
    #define TLO_IPINFO_MAX_CITY_LEN (16+1)
    #define TLO_IPINFO_MAX_REGION_LEN (16+1)
    #define TLO_IPINFO_MAX_COUNTRY_LEN (2+1)
    #define TLO_IPINFO_MAX_POSTAL_LEN (8+1)
    #define TLO_IPINFO_MAX_TIMEZONE_LEN (29+1)
    #define TLO_IPINFO_MAX_TOKEN_LEN (14+1)

    #define TLO_MAX_LONG_DATETIME_LEN (36+1)
    #define TLO_MAX_POSIX_LEN (32+1)                // TODO - needs confirming
    

    // Forward Declarations
    class Timezone;


    /** @class Location
     *  @brief Data struct class to store location details */
    class Location {

        public:

            // Longitude and Latitude coordintes
            struct Coordinate {
                float lon;      // Longitude
                float lat;      // Latitude
            };

            /** Resets location to default */
            void SetDefaults();

            IPAddress publicIP;                                 // Public IP address
            char city[TLO_IPINFO_MAX_CITY_LEN];                 // City
            char region[TLO_IPINFO_MAX_REGION_LEN];             // Region
            char country[TLO_IPINFO_MAX_COUNTRY_LEN];           // Country
            Coordinate coords;                                  // Coordinates      
            char postal[TLO_IPINFO_MAX_POSTAL_LEN];             // Postal code
            char timezone[TLO_IPINFO_MAX_TIMEZONE_LEN];         // Timezone in standard Oslen form

             // Create a compare operators
             
            bool operator== ( const Location& other ) const {
                return ( strcmp( city, other.city ) == 0 )
                    && ( strcmp( region, other.region ) == 0 )
                    && ( strcmp( country, other.country ) == 0 )
                    && coords.lon == other.coords.lon
                    && coords.lat == other.coords.lat
                    && ( strcmp( postal, other.postal ) == 0 )
                    && ( strcmp( timezone, other.timezone ) == 0 );
            }
            bool operator!= ( const Location& other ) const {
                return ( strcmp( city, other.city ) != 0 )
                    || ( strcmp( region, other.region ) != 0 )
                    || ( strcmp( country, other.country ) != 0 )
                    || coords.lon != other.coords.lon
                    || coords.lat != other.coords.lat
                    || ( strcmp( postal, other.postal ) != 0 )
                    || ( strcmp( timezone, other.timezone ) != 0 );
            }

    };


    /** @class TimeLocationSettings
     *  @brief Data struct class to store time and location settings */
    class TimeLocationSettings {

        public:

            /** Restests settings to defaults */
            void ICACHE_FLASH_ATTR SetDefaults();

            bool enabled;                                   // Is the NTP service running
            char ipInfoToken[TLO_IPINFO_MAX_TOKEN_LEN];     // The token to use at IPInfo.io service
            Location location;                              // The location of the device
            char posix[TLO_MAX_POSIX_LEN];                  // The timezone in posix format

             // Create a compare operators

            bool operator== ( const TimeLocationSettings& other ) const {
                return enabled == other.enabled
                    && ( strcmp( ipInfoToken, other.ipInfoToken ) == 0 )
                    && location == other.location
                    && ( strcmp( posix, other.posix ) == 0 );
            }
            bool operator!= ( const TimeLocationSettings& other ) const {
                return enabled != other.enabled
                    || ( strcmp( ipInfoToken, other.ipInfoToken ) != 0 )
                    || location != other.location
                    || ( strcmp( posix,other.posix ) != 0 );
            }

    };


    /** @class TimeLocationManager
     *  @brief Manages the time and location services */
    class TimeLocationManager {

        public:
            
            /** Initializes the Time and Location serices
             * @param client        // Reference for a reusable WiFiClient
             * @param settings      // Reference for the settings */
            void ICACHE_FLASH_ATTR Begin( WiFiClient& client, TimeLocationSettings &settings );

            /** Restart the time and location services 
             * @param settings      // Reference for the settings to use */
            void ICACHE_FLASH_ATTR Restart( TimeLocationSettings &settings );

            /** Handles any repeasting time and location tasks */
            void Handle();

            /** Detect location using IPInfo.io */
            bool ICACHE_FLASH_ATTR DetectLocation();
            
            /** Update the time using NTP */
            void ICACHE_FLASH_ATTR UpdateTime();

            /** Is the current time set on the device
             *  @return true:       Time is set
             *  @return false:      Time is not set */
            bool ICACHE_FLASH_ATTR IsTimeSet() { return _isTimeSet; }

            /** Is the current location set on the device 
             * @return true         Location is set
             * @return false        Location is not set */
            bool ICACHE_FLASH_ATTR IsLocationSet() { return _isLocationSet; }

            /** Copy the time date into a char array
             *  @param datetimestring        Destination */
            void ICACHE_FLASH_ATTR StrcpyTimeDate( char* datetimestring );


        protected:

            WiFiClient* _client;                        // Pointer to the reusable WiFi Client
            TimeLocationSettings* _settings;            // Pointer to the time and locaiton settings 
            Timezone* _timezone;                        // Pointer to the timezone object (created in Begin function)
            bool _isTimeSet = false;                    // Is the current time set
            bool _isLocationSet = false;                // Is the current location set
            bool _wasPreviousConnected = false;         // Have we had an internet connection before        

    };


    extern TimeLocationManager timelocation;            // Global instance of the time and location service

#endif      // TIME_LOCATION_H



