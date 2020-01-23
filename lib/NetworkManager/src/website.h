/* Website Manager Library

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


*/



#ifndef WEBSITE_MANAGER_H

    #define WEBSITE_MANAGER_H

    #include <ESP8266WebServer.h>
    #include "EmbAJAX.h"

    #define WEB_PORT 80
    

    struct PageHandler {
        const char* URL;
        void (*handler)();
        void (*init)();
    };



    // Website Manager Class

    class WebsiteManager {

        public:

            WebsiteManager()
                : _server(WEB_PORT)
                , _ajax(&_server)
            {}

            EmbAJAXOutputDriverWebServerClass _server;
            EmbAJAXOutputDriver _ajax;

            void begin();
            void handle() { _ajax.loopHook(); };


        protected:

            String getContentType( const String filename );         // convert the file extension to the MIME type
            bool handleSPIFFS( const String path );                 // send the right file to the client (if it exists)
            void handleAJAX( const String path );                   // process AJAX request
            void handleInit( const String path );                   // process page initialization


        private:


    };


    extern WebsiteManager website;        // Declaring the global instance


#endif