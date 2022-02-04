/**
 * @file        ThingManager.h
 * @author      Chris Gregg
 * 
 * @brief       Defines and manages the connection to Thinger.io
 * 
 * @copyright   Copyright (c) 2022
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


#ifndef THING_MANAGER_H

    #define THING_MANAGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <WiFiClient.h>

    // Define Sizes
    #define THINGER_USER_MAX_LEN 16
    #define THINGER_DEVICE_MAX_LEN 16
    #define THINGER_TOKEN_MAX_LEN 20

    // Defaults
    #define THINGER_DEFAULT_MODE true

    // Forward Declarations
    class ThingerClient;

    /** @class OTAUpdaterSettings
     *  @brief Data struct class containing settings for OTA update service */
    class ThingerSettings
    {
            
        public:

            /** Resets the settings to default */
            void ICACHE_FLASH_ATTR SetDefaults();

            bool enabled;                           // Is the Thinger.io service on
            char user[THINGER_USER_MAX_LEN];        // Thinger.io username
            char device[THINGER_DEVICE_MAX_LEN];    // Thinger.io device name
            char token[THINGER_TOKEN_MAX_LEN];      // Thinger.io device token

            // Create a compare operators

            bool operator== ( const ThingerSettings &other ) const {
                return enabled == other.enabled
                    && ( strcmp( user, other.user ) == 0 )
                    && ( strcmp( device, other.device ) == 0 )
                    && ( strcmp( token, other.token ) == 0 );
            }
            bool operator!= ( const ThingerSettings &other ) const {
                return enabled != other.enabled
                    || ( strcmp( user, other.user ) != 0 )
                    || ( strcmp( device, other.device ) != 0 )
                    || ( strcmp( token, other.token ) != 0 );
            }
    };


    /** @class Thing Manager Class
     *  @brief Wrapping the Thinger.io client class for this environment */
    class ThingManager {

        public:

            /** Sets up the device hardware and build environment
             * @param client            Reference for the resuable WiFi client
             * @param settings          Reference for the settings struct  //TODO
             */
            void ICACHE_FLASH_ATTR Begin( ThingerSettings& settings );

            /** Handles any repeating device actions */ 
            void ICACHE_FLASH_ATTR Handle();

            /** Restart Thinger functions */
            void ICACHE_FLASH_ATTR Restart( ThingerSettings& settings );

            /** Toggle status of LED */
            void ICACHE_FLASH_ATTR ToggleLED();

            void ICACHE_FLASH_ATTR SendLEDUpdate();

        protected:

            ThingerClient* io;
            
            WiFiClient _wifiClient;             // WiFiClient for Thinger.io
            ThingerSettings* _settings;         // Pointer to the data struct holding the Thigner settings

            /** Update status of LED */
            void ICACHE_FLASH_ATTR UpdateLED( );
    };

    extern ThingManager thing;

#endif          // THING_MANAGER_H