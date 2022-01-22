/**
 * @file        WebManager.h
 * @author      Chris Gregg
 * 
 * @brief       Manages the embedded website
 * 
 * Builds on EmbAJAX Library - https://github.com/tfry-git/EmbAJAX
 * Uses either inlined flash files or LittleFS to server web files, and handles AJAX calls
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



#ifndef WEBSITE_MANAGER_H

    #define WEBSITE_MANAGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266WiFi.h>
    #include <EmbAJAX.h>

    // Project Libraries
    #include "TimeLocation.h"


    #define WEB_PORT 80
    #define WEB_MAX_MESSAGE_LEN 100
    

    // Describes a page for the EmbAJAX functionality
    struct PageHandler {
        const char* URL;
        void (*ajaxHander)();
        void (*initHandler)();
    };


    ////////////////////////////////////////////
    //// Additional Element Types

    /** @class  EmbAJAXClientFunction
     *  @brief  A global javasript function that can be called from the server (not the client)
     *          Client returns "" on success */
    class EmbAJAXClientFunction : public EmbAJAXElement {

        public:

            // Status of a function call
            enum FunctionStatus : int {
                NONE,           // Not happned
                SENT,           // Sent, waiting for response
                SUCCESS         // Response received successfully
            };

            // Overrides from EmbAJAXElement

            /** Contructor
             *  @param id       ID for this element */
            EmbAJAXClientFunction( const char* id ) : EmbAJAXElement( id ) {
                setBasicProperty( EmbAJAXBase::HTMLAllowed, false );
                _arg = 0;
            }

            /** Gets the value of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */
            const char* value( uint8_t which ) const;

            /** Gets the value property of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */
            const char* valueProperty( uint8_t which = EmbAJAXBase::Value ) const override;

            /** Gets the value from the AJAX request
             *  @param argname      Name of argment */
            void updateFromDriverArg(const char* argname) override;

            /** Placeholder for required overload */
            void print() const override {} 

            // Additional Functionality
            
            /** Initiates a call of the function 
             *  Only require acknowledge for first call, not response from function 
             *  @param arg       Argument to be passed (char array pointer)
             *  @param ack       Do we need an acknowledgement */
            void Call( char* arg, bool ack = true );

            /** Initiates a call of the function
             *  No arguments or acknowledgement needed */
            void Call();

            /** Resets the function call */
            void Reset() { _status = NONE; }

            /** Returns the status of the function call
             *  @return FunctionStatus */
            FunctionStatus getStatus() { return _status; }


        protected:

            char* _arg;                             // Function argment for the call
            FunctionStatus _status = NONE;          // Status of the function call

    };


    /** @class EmbAJAXVarString
     *  @brief A global char variable that can be updated from the server (not the client) */
    template<size_t SIZE> class EmbAJAXVarString : public EmbAJAXElement {

        public:

            /** Constructor 
             *  @param id       ID for this element
             *  @param initial  Initial value */
            EmbAJAXVarString( const char* id, const char* initial ) : EmbAJAXElement( id ) {
                setBasicProperty( EmbAJAXBase::HTMLAllowed, false );
                strcpy( _value, initial );
            }

            // Overrides from EmbAJAXElement

            /** Gets the value of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */
            const char* value( uint8_t which = EmbAJAXBase::Value ) const override {
                if( which == EmbAJAXBase::Value) return _value;
                return EmbAJAXElement::value( which );
            }

            /** Gets the value property of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */
            const char* valueProperty( uint8_t which = EmbAJAXBase::Value ) const override {
                if( which == EmbAJAXBase::Value ) return "embajax_var";
                return EmbAJAXElement::valueProperty( which );
            }

            /** Gets the value from the AJAX request
             *  @param argname      Name of argment */
            void updateFromDriverArg( const char* argname ) override { _driver->getArg( argname, _value, SIZE ); }

            /** Placeholder for required overload */
            void print() const override {} 

            // Additional Functionality

            /** Sets value of element varialbe
             * @param value     Value */
            void setValue( const char* value ) {
                strncpy( _value, value, SIZE );
                setChanged();
            }


        protected:
            char _value[SIZE];      // Stored value of element

    };
  

    /** @class EmbAJAXVariable
     *  @brief A global char variable that can be updated from the server (not the client) */
    template<typename T> class EmbAJAXVariable : public EmbAJAXElement {

        public:

            /** Constructor 
             *  @param id       ID for this element
             *  @param initial  Initial value */
            EmbAJAXVariable( const char* id, T initial = 0 ) : EmbAJAXElement( id ) {
                setBasicProperty(EmbAJAXBase::HTMLAllowed, false);
                _value = initial;
            }

            // Overrides from EmbAJAXElement

            /** Gets the value of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */
            const char* value(uint8_t which = EmbAJAXBase::Value) const override;

            /** Gets the value property of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */            
            const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override {
                if (which == EmbAJAXBase::Value) return "embajax_var";
                return EmbAJAXElement::valueProperty(which);
            }

            /** Gets the value from the AJAX request
             *  @param argname      Name of argment */
            void updateFromDriverArg(const char* argname) override;
            
            /** Placeholder for required overload */
            void print() const override {} 

            // Additional Functionality

            /** Sets value of element variable
             * @param value     Value */
            void setValue( T value ) {
                if( _value != value ) {
                    _value = value;
                    setChanged();      
                }
            }

            /** gets the value of element variable as an int
             * @return int     Value */
            int GetIntValue() const;

            /** gets the value of element variable as a bool
             * @return book     Value */
            bool GetBoolValue() const;


        protected:
            T _value;       // Stored value of the element

    };

    // Create some easier to use typdefs

    typedef class EmbAJAXVariable<int> EmbAJAXServerFunction;
    typedef class EmbAJAXVariable<int> EmbAJAXVarInt;
    typedef class EmbAJAXVariable<bool> EmbAJAXVarBool;


    /** @class EmbAJAXStyle
     *  @brief An HTML span element with properties that can be updated from the server (not the client) */
    class EmbAJAXStyle : public EmbAJAXMutableSpan {
        
        public:

            /** Constructor 
             *  @param id       ID for this element */
            EmbAJAXStyle( const char* id ) : EmbAJAXMutableSpan( id ) {
                _style = 0;
                setBasicProperty( EmbAJAXBase::HTMLAllowed, false );
            }

            // Overrides from EmbAJAXElement

            /** Gets the value of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */
            const char* value(uint8_t which = EmbAJAXBase::Value) const override;

            /** Gets the value property of the element
             *  @param which         Which value
             *  @return const char*  Pointer to value */   
            const char* valueProperty(uint8_t which = EmbAJAXBase::Value) const override;

            // Adds a new property to the EMBAjaxBase
            enum Property {                                                     
                Style = EmbAJAXBase::FirstElementSpecificProperty
            };

            // Additional Functionality

            /** Sets the style for the element
             *  @param style    Style string to apply to element */
            void setStyle(const char* style);


        protected:

            const char* _style;     // Pointer to stored style string

    };


    // Elements that go on every page and are declared in this lib
    #define WEB_PAGE_COMMON_ELEMENTS &net_status,&post_message,&date_time
    #define WEB_PAGE_COMMON_ELEMENTS_COUNT 3



    /** @class WebsiteManager
     *  @brief Manages website tasks */
    class WebsiteManager {

        public:

            /** Constructor */
            WebsiteManager()
                : _server( WEB_PORT )           // Initialize webserver
                , _ajax( &_server )             // Initialze EmbAJAX
            {}

            /** Initializes the web manager
             * @param hostname      Hostname to use in the user agent */
            void ICACHE_FLASH_ATTR Begin( char* hostname );

            /** Handle repeating web server tasks */
            void Handle() { _ajax.loopHook(); }

            /** Send message to web client message popup
             * @param msg       Message to send (char array) */
            void ICACHE_FLASH_ATTR PostMessage( const char* msg );

            /** Send message to web client message popup
             * @param msg       Message to send (String) */
            void ICACHE_FLASH_ATTR PostMessage( String msg );
 
            String URL;         // URL of web request
            String AjaxID;      // ID of AJAX request
            String AjaxValue;   // Value of AJAX request


        protected:

            /** Redirect captive portal request back to the root */
            void ICACHE_FLASH_ATTR RedirectCaptivePortal();

            /** Determine the MIME content type from the extension
             * @param filename      File name as a string
             * @return String       Content type */
            String ICACHE_FLASH_ATTR GetContentType( const String filename );
            
            /** Send the file to the client (if it exists)
             *  @return true:       File found
             *  @return false:      File not found */
            bool ICACHE_FLASH_ATTR HandleFileRequest();

            /** Process AJAX request */
            void ICACHE_FLASH_ATTR HandleAJAX();

            /** Process page initialization */
            void ICACHE_FLASH_ATTR InitAJAX();

            /** Detect if request is from a captive portal 
             * @return true:        Request from captive portal
             * @return false:       Request is not from a captive portal */
            bool ICACHE_FLASH_ATTR CheckCaptivePortal();

            char _message[WEB_MAX_MESSAGE_LEN];                 // Stored copy of message
            char _dateTime[TLO_MAX_LONG_DATETIME_LEN];          // Store the date time string for the footer

            char * _hostName;                                   // Host name on the network

            WiFiClient* _client;                                // Pointer to reusable WiFi client
            ESP8266WebServer _server;                           // Web server instance
            EmbAJAXOutputDriver _ajax;                          // EmbAJAX instance

    };

    // Global elements used on every page

    extern EmbAJAXVarInt net_status;                // Status indicator state
    extern EmbAJAXClientFunction post_message;      // Message popup function
    extern EmbAJAXMutableSpan date_time;            // Date and time element

    extern WebsiteManager website;                  // Declaring the global instance

#endif          // WEBSITE_MANAGER_H