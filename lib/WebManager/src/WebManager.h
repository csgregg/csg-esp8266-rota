/* Website Manager Library

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

Builds on EmbAJAX Library - https://github.com/tfry-git/EmbAJAX

Uses either inlined flash files or LittleFS to server web files, and handles AJAX calls

*/



#ifndef WEBSITE_MANAGER_H

    #define WEBSITE_MANAGER_H

    #include <Arduino.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266WiFi.h>
    #include <EmbAJAX.h>

    #include "TimeLocation.h"

    #define WEB_PORT 80
    #define WEB_MAX_MESSAGE_LEN 100
    

    struct PageHandler {
        const char* URL;
        void (*handler)();
        void (*init)();
    };


    ////////// Additional Element Types //////////

    /** @brief A global javasript function that can be called from the server (not the client) */
    /** Client returns "" on success **/
    class EmbAJAXClientFunction : public EmbAJAXElement {
    public:

        enum funcStatus : int {
            NONE,
            SENT,
            SUCCESS
        };

        EmbAJAXClientFunction(const char* id) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            _arg = 0;
        }
        void print() const override { return; } 
        const char* value(uint8_t which) const {
            if (which == EmbAJAXBase::Value) return _arg;
            return EmbAJAXElement::value(which);
        }
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return "embajax_func";
            return EmbAJAXElement::valueProperty(which);
        }
        
        void call(char* arg, bool ack = true ) {            // Only require acknowledge for first call, not response from function 
            _arg = arg;
            if( ack ) _status = SENT;
            else _status = NONE;
            setChanged();
        }
        void call() {                   // Only require acknowledge for first call, not response from function 
            _arg = '\0';
            _status = NONE;
            setChanged();
        }

        void reset() { _status = NONE; }
        void updateFromDriverArg(const char* argname) override {
            char buff[1];
            _driver->getArg(argname, buff, 1);
            if( _status == SENT && buff[0]==0 ) _status = SUCCESS;         // Function should return "" to acknowledge
        }
        funcStatus getStatus() { return _status; }



    private:
        char* _arg;
        funcStatus _status = NONE;
    };


      /** @brief A global char variable that can be updated from the server (not the client) */
    template<size_t SIZE> class EmbAJAXVarChar : public EmbAJAXElement {
    public:
        EmbAJAXVarChar(const char* id, const char* initial) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            strcpy(_value, initial);
        }
        void print() const override {}
        const char* value(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return _value;
            return EmbAJAXElement::value(which);
        }
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return "embajax_var";
            return EmbAJAXElement::valueProperty(which);
        }
        void setValue(const char* value) {
            strncpy(_value, value, SIZE);
            setChanged();
        }
        void updateFromDriverArg(const char* argname) override {
            _driver->getArg(argname, _value, SIZE);
        }

    private:
        char _value[SIZE];
    };
  

    /** @brief A global char variable that can be updated from the server (not the client) */
    template <typename T> class EmbAJAXVariable : public EmbAJAXElement {
    public:
        EmbAJAXVariable(const char* id, T initial = 0) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            _value = initial;
        }
        void print() const override {} 
        const char* value(uint8_t which = EmbAJAXBase::Value) const override;
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return "embajax_var";
            return EmbAJAXElement::valueProperty(which);
        }
        void updateFromDriverArg(const char* argname) override {

        }
        void setValue(T value) {
            if ( _value != value ) {
                _value = value;
                setChanged();            
            }
        }
        int intValue() const;
        bool boolValue() const;

    private:
        T _value;
    };

    typedef class EmbAJAXVariable<int> EmbAJAXServerFunction;
    typedef class EmbAJAXVariable<int> EmbAJAXVarInt;
    typedef class EmbAJAXVariable<bool> EmbAJAXVarBool;


    /** @brief An HTML span element with properties that can be updated from the server (not the client) */
    class EmbAJAXStyle : public EmbAJAXMutableSpan {
    public:
        EmbAJAXStyle(const char* id) : EmbAJAXMutableSpan(id) {
            _style = 0;
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
        }

        const char* value(uint8_t which = EmbAJAXBase::Value) const override;
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override;
        void setStyle(const char* style);
        enum Property {                                                     // Adds a new property to the EMBAjaxBase
            Style = EmbAJAXBase::FirstElementSpecificProperty
        };
    private:
        const char* _style;
    };


    // Elements that go on every page and are declared in this lib
    #define WEB_PAGE_COMMON_ELEMENTS &net_status,&post_message,&date_time
    #define WEB_PAGE_COMMON_ELEMENTS_COUNT 3



    ////////// Website Manager Class ///////////

    class WebsiteManager {

        public:

            WebsiteManager()
                : _server(WEB_PORT)
                , _ajax(&_server)
            {}

            ESP8266WebServer _server;
            EmbAJAXOutputDriver _ajax;

            void ICACHE_FLASH_ATTR begin( char* hostname);
            void handle() { _ajax.loopHook(); };
            void ICACHE_FLASH_ATTR postMessage(const char* msg);
            void ICACHE_FLASH_ATTR postMessage(String msg);

            void ICACHE_FLASH_ATTR redirectToCaptivePortal();
            void ICACHE_FLASH_ATTR setHostname(char* hostname) {
                _hostname = hostname;
            };
 
            String URL;
            String AjaxID;
            String AjaxValue;


        protected:

            String ICACHE_FLASH_ATTR getContentType( const String filename );         // convert the file extension to the MIME type
            bool ICACHE_FLASH_ATTR handleFileRequest();                               // send the right file to the client (if it exists)
            void ICACHE_FLASH_ATTR handleAJAX();                                      // process AJAX request
            void ICACHE_FLASH_ATTR InitAJAX();                                        // process page initialization

            char _message[WEB_MAX_MESSAGE_LEN];                                     // Stored copy of message
            char _datetime[TLO_MAX_LONG_DATETIME_LEN];                                  // Store the date time string for the footer

            bool ICACHE_FLASH_ATTR checkCaptivePortal();                            // Detect captive portal
            char * _hostname;

            WiFiClient* _client;

        private:


    };

    extern EmbAJAXVarInt net_status;
    extern EmbAJAXClientFunction post_message;
    extern EmbAJAXMutableSpan date_time;
    extern WebsiteManager website;        // Declaring the global instance

#endif