/**
 * @file        WebManager.cpp
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


// Global Libraries


// Project Libraries
#include "WebManager.h"
#include "Logger.h"
#include "TimeLocation.h"
#include "NetworkManager.h"

#include "WebFiles.h"
#include "Website.h"


////////////////////////////////////////////
//// EmbAJAXClientFunction Class

// Public:

// Gets the value of the element
const char* EmbAJAXClientFunction::value( uint8_t which ) const {
    if( which == EmbAJAXBase::Value ) return _arg;
    return EmbAJAXElement::value( which );
}


// Gets the value property of the element
const char* EmbAJAXClientFunction::valueProperty( uint8_t which ) const {
    if( which == EmbAJAXBase::Value ) return "embajax_func";
    return EmbAJAXElement::valueProperty( which );
}


// Gets the value from the AJAX request
void EmbAJAXClientFunction::updateFromDriverArg(const char* argname) {
    char buff[1];
    _driver->getArg( argname, buff, 1 );
    if( _status == SENT && buff[0] == 0 ) _status = SUCCESS;         // Function should return "" to acknowledge
}


// Initiates a call of the function
void EmbAJAXClientFunction::Call( char* arg, bool ack ) {
    _arg = arg;
    if( ack ) _status = SENT;
    else _status = NONE;
    setChanged();               // Triggers the response to client
}


// Initiates a call of the function
void EmbAJAXClientFunction::Call() {
    _arg = '\0';
    _status = NONE;
    setChanged();               // Triggers the response to client
}


////////////////////////////////////////////
//// EmbAJAXVariable Class

// Public:

// Gets the value of the element <type char*>
template<> const char* EmbAJAXVariable<char*>::value( uint8_t which ) const {
    if( which == EmbAJAXBase::Value ) return _value;
    return EmbAJAXElement::value( which );
}


// Gets the value of the element <type int>
template<> const char* EmbAJAXVariable<int>::value( uint8_t which ) const {
    if( which == EmbAJAXBase::Value ) {
        itoa( _value, itoa_buf, 10 );
        return itoa_buf;
    }
    return EmbAJAXElement::value( which );
}


// Gets the value of the element <type bool>
template<> const char* EmbAJAXVariable<bool>::value( uint8_t which ) const {
    if( which == EmbAJAXBase::Value ) {
        return _value ? "t" : "f";
    }
    return EmbAJAXElement::value( which );
}


// Gets the value from the AJAX request <char *>
template<> void EmbAJAXVariable<char*>::updateFromDriverArg( const char* argname ) {
    _driver->getArg( argname, _value, sizeof(itoa_buf) );
}


// Gets the value from the AJAX request <int>
template<> void EmbAJAXVariable<int>::updateFromDriverArg(const char* argname) {
    _value = atoi( _driver->getArg( argname, itoa_buf, sizeof(itoa_buf) ) );
}


// Gets the value from the AJAX request <bool>
template<> void EmbAJAXVariable<bool>::updateFromDriverArg(const char* argname) {
    _driver->getArg( argname, itoa_buf, sizeof(itoa_buf) );
    _value = ( strcmp( itoa_buf, "t" ) == 0 );
}


// Gets the value as an int <int>
template<> int EmbAJAXVariable<int>::GetIntValue() const {
    return _value;
}


// Gets the value as an int <char*>
template<> int EmbAJAXVariable<char*>::GetIntValue() const {
    return atoi( _value );
}


// Gets the value as an int <bool>
template<> int EmbAJAXVariable<bool>::GetIntValue() const {
    return _value;
}


// Gets the value as an bool <int>
template<> bool EmbAJAXVariable<int>::GetBoolValue() const {
    return ( _value == 1 );
}


// Gets the value as an bool <char*>
template<> bool EmbAJAXVariable<char*>::GetBoolValue() const {
    return ( strcmp( _value, "t" ) == 0 );
}


// Gets the value as an bool <bool>
template<> bool EmbAJAXVariable<bool>::GetBoolValue() const {
    return _value;
}


////////////////////////////////////////////
//// EmbAJAXStyle Class

// Public:

// Gets the value of the element
const char* EmbAJAXStyle::value( uint8_t which ) const {
    if( which == EmbAJAXStyle::Style ) return _style;
    return EmbAJAXMutableSpan::value( which );
}


// Gets the value property of the element
const char* EmbAJAXStyle::valueProperty( uint8_t which ) const {
    if( which == EmbAJAXStyle::Style ) return "style";
    return EmbAJAXMutableSpan::valueProperty( which );
}


// Sets the style for the element
void EmbAJAXStyle::setStyle( const char* style ) {
    _style = style;
    setChanged();
}


////////////////////////////////////////////
//// WebsiteManager Class

// Public:

// Initialize web manaber
void ICACHE_FLASH_ATTR WebsiteManager::Begin( char* hostName ) {

    _hostName = hostName;

     // If the client requests any URI
    _server.onNotFound( 

        [&]() { 

            URL = _server.uri();
            AjaxID = _server.arg( "id" );
            AjaxValue = _server.arg( "value" );    

            if( URL.endsWith( F("/") ) ) URL += F("index.html");         // If a folder is requested, send the index file
            if( !URL.startsWith( F("/") ) ) URL =+ F("/");

            // AJAX request
            if( _server.method() == HTTP_POST ) {

                // Poll requests only
                if( AjaxID == "" ) {
                    // Update status icon
                    net_status.setValue( network.GetNetworkStatus() );

                    // Clear the message and don't need acknowledgement
                    if( post_message.getStatus() == EmbAJAXClientFunction::SUCCESS ) post_message.Call();

                    // Update date time string
                    if( timelocation.IsTimeSet() ) timelocation.StrcpyTimeDate(_dateTime);                              
                    else strcpy_P( _dateTime, PSTR("Time not set") );
                    date_time.setValue( _dateTime );
                }

                // Find and call AJAX handler
                for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ ) {
                    // TODO - do we need to make it case insensitive?
                    if( URL == webpages[i].URL ) {                                    
                        (webpages[i].ajaxHander)();            // Call page event handler
                        break;
                    }
                }

                return;
            }

            // Page request - send it if it exists otherwise.
            if( HandleFileRequest() ) return;
            
            // Check for captive portal requests and redirecct
            if( CheckCaptivePortal() ) {
                LOG_HIGH( PSTR("(Website) Captive Portal Detection") );
                return;
            }

            LOGF( PSTR("(Website) Web server - File not found: %s"), URL.c_str() );

            // Respond with a 404 (Not Found) error if nothing else works
            _server.send( 404, PSTR("text/html"), PSTR("404: Not Found") );
        
        }

    );

    _server.begin();            // Start webserver

    // Initialize all page Ajax
    for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ )
        (webpages[i].initHandler)();

}


// Detect if request is from a captive portal
bool ICACHE_FLASH_ATTR WebsiteManager::CheckCaptivePortal() {

    char redirectto[DNS_MAX_HOSTNAME_LEN+sizeof("http://.local/")];
    strcpy( redirectto, PSTR("http://") );
    strcat( redirectto, _hostName );
    strcat_P( redirectto, PSTR(".local/") );

    // Windows - redirect
    if( URL.endsWith( PSTR("/ncsi.txt") ) ) {
        _server.send ( 200, PSTR("text/plain"), PSTR("") );
        return true;
    }
    if( URL.endsWith( PSTR("/redirect") ) ) {
        _server.sendHeader( PSTR("Location"), redirectto, true );
        _server.setContentLength(0);
        _server.send ( 302, PSTR("text/plain"), PSTR("") );
        return true;
    }

    // Apple - block captive pop up                                 // TODO - figure out how to use captive portal
    if( URL.endsWith(PSTR("/hotspot-detect.html")) ) {
        _server.send(200, PSTR("text/plain"), PSTR("<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>"));

        return true;
    }

    // Not captive portal
    return false;
}  


// convert the file extension to the MIME type
String ICACHE_FLASH_ATTR WebsiteManager::GetContentType( String filename ) { 
    if( filename.endsWith( PSTR(".html") ) ) return PSTR("text/html");
    else if( filename.endsWith( PSTR(".css") ) ) return PSTR("text/css");
    else if( filename.endsWith( PSTR(".js") ) ) return PSTR("application/javascript");
    else if( filename.endsWith( PSTR(".ico") ) ) return PSTR("image/x-icon");
    else if( filename.endsWith( PSTR(".gif") ) ) return PSTR("image/gif");
    else if( filename.endsWith( PSTR(".svg") ) ) return PSTR("image/svg+xml");
    else if( filename.endsWith( PSTR(".jpg") ) ) return PSTR("image/jpeg");
    else if( filename.endsWith( PSTR(".png") ) ) return PSTR("image/png");
    return PSTR("text/plain");
}


// Send the right file to the client (if it exists)
// Assumes GZIPed files
bool ICACHE_FLASH_ATTR WebsiteManager::HandleFileRequest() {

    String contentType = GetContentType(URL);                  // Get the MIME type

    // Try from Flash
    for( uint i=0; i<(sizeof(websiteFiles)/sizeof(t_websitefiles)); i++ ) {
        if( strcmp_P( URL.c_str(), websiteFiles[i].path ) == 0 ) {

            LOGF_HIGH( PSTR("(Website) Web server - flash: %s"), URL.c_str() );

            _server.sendHeader(PSTR("Content-Encoding"),PSTR("gzip"));
            _server.setContentLength(websiteFiles[i].len);
            _server.send(200,contentType,PSTR(""));
            _server.client().write_P((const char*)FPSTR(websiteFiles[i].content),websiteFiles[i].len);

            // Initialize Ajax on page load
            for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ )
                if( URL == webpages[i].URL ) {
                    (webpages[i].initHandler)();
                    break;
                }

            return true;
        }
    }

    return false;                                         // If the file doesn't anywhere, return false
}


// Send message to web client message popup (char array)
void ICACHE_FLASH_ATTR WebsiteManager::PostMessage( const char* msg ) {
    strncpy( _message, msg, WEB_MAX_MESSAGE_LEN );
    post_message.Call( _message );
}


// Send message to web client message popup (String)
void ICACHE_FLASH_ATTR WebsiteManager::PostMessage( String msg ){
    strncpy( _message, msg.c_str(), WEB_MAX_MESSAGE_LEN );
    post_message.Call( _message );
}


// Protected:

// Redirect captive portal request back to the root
void ICACHE_FLASH_ATTR WebsiteManager::RedirectCaptivePortal() {

    LOG( PSTR("(Website) Request redirected to captive portal") );

    // Full local URL of device
    char buffer[DNS_MAX_HOSTNAME_LEN+sizeof("http://")];
    strcpy_P( buffer, PSTR("http://") );
    strncat( buffer, network.GetHostName(), sizeof(buffer) );

    // Respond with redirecg
    _server.sendHeader( PSTR("Location"), buffer, true );
    _server.setContentLength( 0 );
    _server.send ( 302, PSTR("text/plain"), PSTR("") );

}


// Create the global instances
EmbAJAXVarInt net_status("net_status",0);
EmbAJAXClientFunction post_message("post_message");
EmbAJAXMutableSpan date_time("date_time");
WebsiteManager website;
