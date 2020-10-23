/**
 * @file        OTAUpdater.h
 * @author      Chris Gregg
 * 
 * @brief       Implements a remote update service for ESP using GitHub
 *              to store code and binaries built by Travis-CI.
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


#ifndef OTA_UPDATER_H

    #define OTA_UPDATER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WiFi.h>
    #include <ESP8266httpUpdate.h>
    #include <Ticker.h>

    // Define Sizes
    #define OTA_MAX_SERVICE_LEN 48
    #define OTA_MAX_REPO_LEN 24
    #define OTA_MAX_USER_LEN 16
    #define OTA_MAX_TOKEN_LEN 48
    #define OTA_MAX_CODE_LEN 16
    #define OTA_MAX_TAG_LEN 12
    #define OTA_MAX_DATE_LEN 22
    #define OTA_MAX_URL_LEN (OTA_MAX_SERVICE_LEN + OTA_MAX_REPO_LEN + OTA_MAX_USER_LEN + OTA_MAX_TOKEN_LEN + OTA_MAX_CODE_LEN + OTA_MAX_TAG_LEN + (sizeof("http://") - 1) + (sizeof("?repo=") - 1) + (sizeof("&user=") - 1) + (sizeof("&token=") - 1) + 1)
    #define OTA_MAX_IMG_URL_LEN (OTA_MAX_URL_LEN + (sizeof("&asset=") - 1) + (sizeof(flag_DEVICE_CODE) - 1) + (sizeof("-Pv") - 1) + (sizeof("&tag=") - 1) + OTA_MAX_TAG_LEN + (sizeof("&type=gz") - 1))

    // Defaults
    #define OTA_DEFAULT_MODE true


    /** @class OTAUpdaterSettings
     *  @brief Data struct class containing settings for OTA update service */
    class OTAUpdaterSettings
    {
            
        public:

            /** Resets the settings to default */
            void ICACHE_FLASH_ATTR SetDefaults();

            bool enabled;                           // Is the update service on
            char service[OTA_MAX_SERVICE_LEN];      // URL for update service
            char repo[OTA_MAX_REPO_LEN];            // GitHub repo
            char user[OTA_MAX_USER_LEN];            // GitHub user
            char token[OTA_MAX_TOKEN_LEN];          // GitHub Token
            uint interval;                          // Check for update interval
            bool skipUpdates;                       // Are we going to skip the update if it is there

            // Create a compare operators

            bool operator== ( const OTAUpdaterSettings &other ) const {
                return enabled == other.enabled
                    && ( strcmp( service, other.service ) == 0 )
                    && ( strcmp( repo, other.repo ) == 0 )
                    && ( strcmp( user, other.user ) == 0 )
                    && ( strcmp( token, other.token ) == 0 )
                    && interval == other.interval
                    && skipUpdates == other.skipUpdates;
            }
            bool operator!= ( const OTAUpdaterSettings &other ) const {
                return enabled == other.enabled
                    || ( strcmp( service, other.service ) != 0 )
                    || ( strcmp( repo, other.repo ) != 0 )
                    || ( strcmp( user, other.user ) != 0 )
                    || ( strcmp( token, other.token ) != 0 )
                    || interval != other.interval
                    || skipUpdates != other.skipUpdates;
            }
    };



    // Remote Updater Class

    /** @class  OTAUpdater
     *  @brief  Manage the Over-The-Air update of the firmware
     *          
     *          Can be compiled with or without a separate file system image.
     *          With a file system image, the web files are stored in a LittlFS image
     *          Without a file system image, the web files are stored are part of the program memory* 
     */
    class OTAUpdater
    {

        public:

            // The result of the update
            enum UpdateResult : int
            {
                FS_UPDATE_FAILED,       // The file system update failed
                PROG_UPDATE_FAILED,     // The program update failed
                NO_UPDATES,             // There were no updates available
                UPDATE_SKIPPED,         // There was an available update but it was skipped
                UPDATE_OK               // An update was performed successfully
            };

            /** Initializes the OTA updater service
             * @param client            Reference for the resuable WiFi client
             * @param settings          Reference for the settings struct
             */
            void ICACHE_FLASH_ATTR Begin( WiFiClient& client, OTAUpdaterSettings& settings );

            /** Reloads the settings for the OTA service
             * @param settings          Reference for the settings struct */
            void ICACHE_FLASH_ATTR Restart( OTAUpdaterSettings& settings );

            /** Handles any repeating tasks for the OTA update service */
            void Handle();


        protected:

            // Type of update file - raw or GZip compressed
            enum BinType
            {
                RAW, // Uncompressed BIN file (.bin)
                GZ   // Compressed BIN file (.bin.gz)
            };

            /** Perform the update check
             * @return true         Update is avaiable
             * @return false        Update is not available
             */
            bool ICACHE_FLASH_ATTR IsUpdateAvailable();

            /** Function called by Ticker interrupt */
            static void TriggerUpdateCheck() { _doUpdateCheck = true; }

            char _assetRequestURL[OTA_MAX_URL_LEN];         // URL of asset request service
            char _latestTag[OTA_MAX_TAG_LEN];               // Tag of latest release
            char _latestReleaseDate[OTA_MAX_DATE_LEN];      // Date of latest release

            WiFiClient *_client;                            // Pointer to reusable WiFiClient
            OTAUpdaterSettings *_settings;                  // Pointer to settings data struct

            Ticker _updateCheck;                            // Ticker object to trigger check events
            static bool _doUpdateCheck;                     // Flag that indicates time to run update

        #ifndef WEB_FLASHFILES // Are we using flash instead of LittleFS for web files
            /** Update the file system firmware from image stored at GitHub
             * @param type              Type of image file, raw or GZipped
             * @return UpdateResult     Result of the update */
            UpdateResult ICACHE_FLASH_ATTR UpdateFS( const bin_type type );
        #endif

            /** Update the program firmware from image stored at GitHub
             * @param type              Type of image file, raw or GZipped
             * @param doRestart         Force restart of device after update
             * @return UpdateResult     Result of the update */
            UpdateResult ICACHE_FLASH_ATTR UpdateProg( const BinType type, const bool doRestart = false );

    };


    extern OTAUpdater updater;        // Global instance of the OTA update service

#endif          // OTA_UPDATER_H
