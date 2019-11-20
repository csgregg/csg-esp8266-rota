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


#include <ESP8266httpUpdate.h>

#ifndef ESP_REMOTE_UPDATER_H

    #define ESP_REMOTE_UPDATER_H



    // Remote Updater Class

    class ESPRemoteUpdater {
        
        public:

            ESPRemoteUpdater();
            
            void begin( HTTPClient &http, WiFiClient &client );

        protected:

            const char* progSuffix = "-Pv";
            const char* FSSuffix = "-Fv";


        private:
        

    };

    extern ESPRemoteUpdater updater;

#endif


