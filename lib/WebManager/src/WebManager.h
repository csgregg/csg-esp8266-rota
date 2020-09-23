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

Uses either inlined flash files or LittleFS to server web files, and hangles AJAX calls

*/



#ifndef WEBSITE_MANAGER_H

    #define WEBSITE_MANAGER_H

    #include <ESP8266WebServer.h>
    #include <EmbAJAX.h>

    #define WEB_PORT 80
    
    struct PageHandler {
        const char* URL;
        void (*handler)();
        void (*init)();
    };

    ////////// Additional Element Types //////////

    /** @brief A global javasript function that can be called from the server (not the client) */
    template <typename T> class EmbAJAXClientFunction : public EmbAJAXElement {
    public:
        EmbAJAXClientFunction(const char* id) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            _arg = 0;
        }
        void print() const override { return; } 
        const char* value(uint8_t which = EmbAJAXBase::Value) const override;
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return "embajax_func";
            return EmbAJAXElement::valueProperty(which);
        }
        void call(T arg) {
            _arg = arg;
            setChanged();
        }
        bool sendUpdates(uint16_t since, bool first) {
            return EmbAJAXElement::sendUpdates(since, first);
        }

    private:
        T _arg;
    };


      /** @brief A global char variable that can be updated from the server (not the client) */
    template <size_t SIZE> class EmbAJAXVarChar : public EmbAJAXElement {
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
        void updateFromDriverArg(const char* argname) override;
        void setValue(T value) {
            if ( _value != value ) {
                _value = value;
                setChanged();            
            };
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
        enum Property {
            Style = EmbAJAXBase::FirstElementSpecificProperty
        };
    private:
        const char* _style;
    };




    ////////// Website Manager Class ///////////


    #define STATUS_FLASH_SPEED  4       // 4 Seconds

    class WebsiteManager {

        public:

            WebsiteManager()
                : _server(WEB_PORT)
                , _ajax(&_server)
            {}

            ESP8266WebServer _server;
            EmbAJAXOutputDriver _ajax;

            void begin();
            void handle() { _ajax.loopHook(); };
 
            String URL;
            String AjaxID;
            String AjaxValue;


        protected:

            String getContentType( const String filename );         // convert the file extension to the MIME type
            bool handleFileRequest();                                    // send the right file to the client (if it exists)
            void handleAJAX();                                      // process AJAX request
            void InitAJAX();                                        // process page initialization

            int statusFlash = 1;


        private:


    };

    extern EmbAJAXVarInt net_status;
    extern WebsiteManager website;        // Declaring the global instance

#endif