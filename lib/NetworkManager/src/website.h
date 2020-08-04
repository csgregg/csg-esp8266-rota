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

    #include "EmbAJAX.h"

    #define WEB_PORT 80
    
    struct PageHandler {
        const char* URL;
        void (*handler)();
        void (*init)();
    };


    /** @brief A global javasript function that can be called from the server (not the client) */
    template <typename T> class EmbAJAXClientFunction : public EmbAJAXElement {
    public:
        EmbAJAXClientFunction(const char* id) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            _call = false;
            _arg = 0;
        }
        void print() const override { return; } 
        const char* value(uint8_t which = EmbAJAXBase::Value) const override;
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) {
                if( _call ) return "embajax_func";
                else return "";
            }
            return EmbAJAXElement::valueProperty(which);
        }
        void call(const T arg) {
            _call = true;
            _arg = arg;
            setChanged();
        }
        bool sendUpdates(uint16_t since, bool first) {
            bool res = EmbAJAXElement::sendUpdates(since, first);
            _call = false;
            return res;
        }

    private:
        bool _call;
        T _arg;
    };



    /** @brief Calls a lcoal function from the client (not the server) */
    class EmbAJAXServerFunction : public EmbAJAXElement {
    public:
        EmbAJAXServerFunction(const char* id ) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            _response = 0;
            _arg = 0;
        }
        void print() const override { return; } 
        const char* value(uint8_t which = EmbAJAXBase::Value) const override
        {
            if (which == EmbAJAXBase::Value) return _response;
            return EmbAJAXElement::value(which);
        }
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return "embajax_var";
            return EmbAJAXElement::valueProperty(which);
        }
        void updateFromDriverArg(const char* argname) override {
            _driver->getArg(argname, _arg, sizeof(itoa_buf));
        }
        void setReturn(char* res) {
            _response = res;
        }

    private:
        char* _response;
        char* _arg;

    };





    /** @brief A global char variable that can be updated from the server (not the client) */
    template <typename T> class EmbAJAXVariable : public EmbAJAXElement {
    public:
        EmbAJAXVariable(const char* id) : EmbAJAXElement(id) {
            setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
            _value = 0;
        }
        void print() const override { return; } 
        const char* value(uint8_t which = EmbAJAXBase::Value) const override;
        const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
            if (which == EmbAJAXBase::Value) return "embajax_var";
            return EmbAJAXElement::valueProperty(which);
        }
        void updateFromDriverArg(const char* argname) override;
        void setValue(T value) {
            _value = value;
            setChanged();
        }
        T getValue() const {
            return _value;
        }
    private:
        T _value;
    };



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
 
            String URL;
            String AjaxID;
            String AjaxValue;


        protected:

            String getContentType( const String filename );         // convert the file extension to the MIME type
            bool handleSPIFFS();                                    // send the right file to the client (if it exists)
            void handleAJAX();                                      // process AJAX request
            void InitAJAX();                                        // process page initialization


        private:


    };

    extern WebsiteManager website;        // Declaring the global instance

#endif