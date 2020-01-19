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
    

    

    class thiswebpage {

        public:
            EmbAJAXCheckButton check;
            EmbAJAXMutableSpan check_d;

            EmbAJAXBase* page_elements[2] = {&check, &check_d};

            void (*ptr)() = NULL;

            thiswebpage() : 
                check("check", " That option"),
                check_d("check_d"),
                page(page_elements, "This")
                {
                    ptr = (void(*)())(&thiswebpage::update);              // This is a bit nasty 
                };

            void update() {
                check_d.setValue(check.isChecked() ? "checked" : "not checked");
            };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> page;

            String URI;
            String gzURI;

            void install(const char *path, ESP8266WebServer *server) {
                URI = path;
                gzURI = URI + ".gz";
                update();
                server->on(path, [=]() {
                    if (server->method() == HTTP_POST) {  // AJAX request

                        page.handleRequest( ptr );

                    } else {  // Page load

                        page.printPage();
                    /*
                        if(SPIFFS.exists(gzURI)) {
                            File file = SPIFFS.open(gzURI, "r");                 // Open it
                            server->streamFile(file, "text/html");              // And send it to the client
                            file.close();                                       // Then close the file again
                        }
                        else server->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error

                        */
                    }
                });
            }
    };



    class thatwebpage {

        public:
            EmbAJAXCheckButton check;
            EmbAJAXMutableSpan check_d;

            EmbAJAXBase* page_elements[2] = {&check, &check_d};

            void (*ptr)() = NULL;

            thatwebpage() : 
                check("check", " That option"),
                check_d("check_d"),
                page(page_elements, "That")
                {
                    ptr = (void(*)())(&thatwebpage::update);              // This is a bit nasty 
                };

            void update() {
                check_d.setValue(check.isChecked() ? "checked" : "not checked");
            };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> page;

            String URI;
            String gzURI;

            void install(const char *path, ESP8266WebServer *server) {
                URI = path;
                gzURI = URI + ".gz";
                update();
                server->on(path, [=]() {
                    if (server->method() == HTTP_POST) {  // AJAX request
                        page.handleRequest( ptr );
                    } else {  // Page load
                        if(SPIFFS.exists(gzURI)) {
                            Serial.println(gzURI);
                            File file = SPIFFS.open(gzURI, "r");                 // Open it
                            server->streamFile(file, "text/html");              // And send it to the client
                            file.close();                                       // Then close the file again
                        }
                        else server->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
                    }
                });
            }
    };


    // Website Manager Class

    class WebsiteManager {

        public:

            WebsiteManager()
                : _server(WEB_PORT)
                , _ajax(&_server)
            {}

            EmbAJAXOutputDriver _ajax;
            EmbAJAXOutputDriverWebServerClass _server;

            void InitializeWebServer();
            ESP8266WebServer& getWebServer() { return _server; };


        protected:


            thiswebpage _thispage;
            thatwebpage _thatpage;
            
            static void updateUI();

            String getContentType(String filename); // convert the file extension to the MIME type
            bool handleFileRead(String path);       // send the right file to the client (if it exists)

        private:


    };


    extern WebsiteManager website;        // Declaring the global instance


#endif