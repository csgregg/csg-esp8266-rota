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

    typedef enum : int {
        REMOTE_FS_UPDATE_FAILED,
        REMOTE_IMG_UPDATE_FAILED,
        REMOTE_UPDATE_NO_UPDATES,
        REMOTE_UPDATE_OK
    } t_update_result;


    // Remote Updater Class

    class OTAUpdater {
        
        public:
            
            void setup( const String &service, const String &repo, const String &user, const String &token, const String &deviceCode, const String &buildTag, long updateinterval, bool skip );
            void begin( WiFiClient& client );

            void handle();

            String getLatestBuild();

            int getLastError();
            String getLastErrorString();

            enum bin_type {
                RAW,                        // Uncompressed BIN file (.bin)
                GZ                          // Compressed BIN file (.bin.gz)
            };

        protected:

            const char* _progSuffix = "-Pv";
            const char* _FSSuffix = "-Fv";

            String _assetRequestURL;
            String _repoName;
            String _deviceCode;
            String _buildTag;
            String _latestTag;
            bool _skipUpdates;

            WiFiClient * _client;

            Ticker _updateCheck;
            static bool _doUpdateCheck;

            float _updateinterval = DEFAULT_UPDATE_INTERVAL;

            static void TriggerUpdateCheck();

            int _lastError;

            HTTPUpdateResult UpdateFS( const bin_type type );
            HTTPUpdateResult UpdateProg( const bin_type type, const bool restart = false );

        private:
        

    };

    extern OTAUpdater updater;

#endif


