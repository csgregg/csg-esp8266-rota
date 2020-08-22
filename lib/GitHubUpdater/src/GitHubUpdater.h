/* Remote Updater Library

MIT License

Copyright (c) 2019 Chris Gregg

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



#ifndef GITHUB_UPDATER_H

    #define GITHUB_UPDATER_H


    // Updater settings

    class UpdaterSettings {
        public:

            void setDefaults();
    
            long interval;
            bool skipUpdates;
            String service;
            String repoName;
            String user;
            String token;
            String deviceCode;
            String buildTag;
        
            // Create a compare operators
            bool operator==(const UpdaterSettings& other) const {

                return interval == other.interval
                    && skipUpdates == other.skipUpdates
                    && service == other.service
                    && repoName == other.repoName
                    && user == other.user
                    && token == other.token
                    && deviceCode == other.deviceCode
                    && buildTag == other.buildTag;

            }

            bool operator!=(const UpdaterSettings& other) const {

                return interval != other.interval
                    || skipUpdates != other.skipUpdates
                    || service == other.service
                    || repoName == other.repoName
                    || user == other.user
                    || token != other.token
                    || deviceCode != other.deviceCode
                    || buildTag != buildTag;

            }
    };


    // Remote Updater Class

    class GitHubUpdater {
        
        public:
            
            void begin( WiFiClient& client, UpdaterSettings& settings );

            void handle();

            enum bin_type {
                RAW,                        // Uncompressed BIN file (.bin)
                GZ                          // Compressed BIN file (.bin.gz)
            };

        protected:

            const char* _progSuffix = "-Pv";
            const char* _FSSuffix = "-Fv";

            String _assetRequestURL;
            String _latestTag;

            UpdaterSettings* _updatersettings;
            WiFiClient * _client;

            Ticker _updateCheck;
            static bool _doUpdateCheck;

            String getLatestBuild();
            static void TriggerUpdateCheck() {
                _doUpdateCheck = true;
            };

            HTTPUpdateResult UpdateFS( const bin_type type );
            HTTPUpdateResult UpdateProg( const bin_type type, const bool restart = false );

        private:
        

    };

    extern GitHubUpdater updater;

#endif


