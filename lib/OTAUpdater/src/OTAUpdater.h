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

Implements a remote update service for ESP using GitHub to store code and
binaries built by Travis-CI.

*/



#ifndef ESP_REMOTE_UPDATER_H

    #define ESP_REMOTE_UPDATER_H

    #include <ESP8266WiFi.h>
    #include <ESP8266httpUpdate.h>
    #include <Ticker.h>

    #define DEFAULT_UPDATE_INTERVAL 300

    // Define Sizes
    #define OTA_MAX_SERVICE_LEN 48
    #define OTA_MAX_REPO_LEN 24
    #define OTA_MAX_USER_LEN 16
    #define OTA_MAX_TOKEN_LEN 48
    #define OTA_MAX_CODE_LEN 16
    #define OTA_MAX_TAG_LEN 12
    #define OTA_MAX_URL_LEN  OTA_MAX_SERVICE_LEN + OTA_MAX_REPO_LEN + OTA_MAX_USER_LEN + OTA_MAX_TOKEN_LEN + OTA_MAX_CODE_LEN + OTA_MAX_TAG_LEN + (sizeof("http://")-1) + (sizeof("?repo=")-1) + (sizeof("&user=")-1) + (sizeof("&token=")-1) + 1
    #define OTA_MAX_IMAGE_URL_LEN OTA_MAX_URL_LEN + (sizeof("&asset=")-1) + (sizeof(flag_DEVICE_CODE)-1) + (sizeof(_progSuffix)-1) + (sizeof("&tag=")-1) + OTA_MAX_TAG_LEN + (sizeof("&type=gz")-1)

    typedef enum : int {
        REMOTE_FS_UPDATE_FAILED,
        REMOTE_IMG_UPDATE_FAILED,
        REMOTE_UPDATE_NO_UPDATES,
        REMOTE_UPDATE_OK
    } t_update_result;



    class OTASettings {
        public:

            void ICACHE_FLASH_ATTR setDefaults();

            char service[OTA_MAX_SERVICE_LEN] = "";
            char repo[OTA_MAX_REPO_LEN] = "";
            char user[OTA_MAX_USER_LEN] = "";
            char token[OTA_MAX_TOKEN_LEN] = "";
            uint interval = DEFAULT_UPDATE_INTERVAL;
            bool skipUpdates = true;

             // Create a compare operators

            bool operator==(const OTASettings& other) const {
                return (strcmp(service, other.service)==0)
                    && (strcmp(repo, other.repo)==0)
                    && (strcmp(user, other.user)==0)
                    && (strcmp(token, other.token)==0)
                    && interval == other.interval
                    && skipUpdates == other.skipUpdates;
            }

            bool operator!=(const OTASettings& other) const {
                return (strcmp(service, other.service)!=0)
                    || (strcmp(repo, other.repo)!=0)
                    || (strcmp(user, other.user)!=0)
                    || (strcmp(token, other.token)!=0)
                    || interval != other.interval
                    || skipUpdates != other.skipUpdates;
            }

    };



    // Remote Updater Class

    class OTAUpdater {
        
        public:
            
            void ICACHE_FLASH_ATTR begin( WiFiClient& client, OTASettings &settings );

            void handle();

            String ICACHE_FLASH_ATTR getLatestBuild();

            enum bin_type {
                RAW,                        // Uncompressed BIN file (.bin)
                GZ                          // Compressed BIN file (.bin.gz)
            };

        protected:

            const char* _progSuffix = "-Pv";
            const char* _FSSuffix = "-Fv";

            char _assetRequestURL[OTA_MAX_URL_LEN];    
            char _latestTag[OTA_MAX_TAG_LEN];

            WiFiClient* _client;
            OTASettings* _settings;

            Ticker _updateCheck;
            static bool _doUpdateCheck;

            static void TriggerUpdateCheck();

            int _lastError;

#ifndef WEB_FLASHFILES      // Are we using flash instead of LittleFS for web files
            HTTPUpdateResult ICACHE_FLASH_ATTR UpdateFS( const bin_type type );
#endif
            HTTPUpdateResult ICACHE_FLASH_ATTR UpdateProg( const bin_type type, const bool restart = false );

        private:
        

    };

    extern OTAUpdater updater;

#endif


