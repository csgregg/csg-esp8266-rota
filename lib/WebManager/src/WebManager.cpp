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



#ifndef WEB_FLASHFILES
#include <LittleFS.h>
#endif

#include "WebManager.h"
#include "Logger.h"

#include "pages/NetworkSetttingsPage.h"
#include "pages/AboutPage.h"
#include "pages/IndexPage.h"
#include "pages/SystemPage.h"

// Are we using flash instead of LittleFS for web files
#ifdef WEB_FLASHFILES
#include "WebFiles.h"
#endif

    
// Webpages
// ========

// Page handlers
PageHandler webpages[] = {
    {networksettingspage.URL, networksettingspage.handler, networksettingspage.init},
    {aboutpage.URL, aboutpage.handler, aboutpage.init},
    {indexpage.URL, indexpage.handler, indexpage.init},
    {systempage.URL, systempage.handler, systempage.init},
};



//////////////////////// EmbAJAXClientFunction /////////////////////////////

template <> const char* EmbAJAXClientFunction<int>::value(uint8_t which) const {
    if (which == EmbAJAXBase::Value) {
        itoa(_arg,itoa_buf,10);
        return itoa_buf;
    }
    return EmbAJAXElement::value(which);
}

template <> const char* EmbAJAXClientFunction<char*>::value(uint8_t which) const {
    if (which == EmbAJAXBase::Value) return _arg;
    return EmbAJAXElement::value(which);
}


//////////////////////// EmbAJAXVariable /////////////////////////////

template <> const char* EmbAJAXVariable<char*>::value(uint8_t which) const {
    if (which == EmbAJAXBase::Value) return _value;
    return EmbAJAXElement::value(which);
}

template <> const char* EmbAJAXVariable<int>::value(uint8_t which) const {
    if (which == EmbAJAXBase::Value) {
        itoa(_value,itoa_buf,10);
        return itoa_buf;
    }
    return EmbAJAXElement::value(which);
}

template <> const char* EmbAJAXVariable<bool>::value(uint8_t which) const {
    if (which == EmbAJAXBase::Value) {
        return _value ? "t" : "f";
    }
    return EmbAJAXElement::value(which);
}

template <> void EmbAJAXVariable<char*>::updateFromDriverArg(const char* argname) {
    _driver->getArg(argname, _value, sizeof(itoa_buf));
}

template <> void EmbAJAXVariable<int>::updateFromDriverArg(const char* argname) {
    _value = atoi(_driver->getArg(argname, itoa_buf, sizeof(itoa_buf)));
}

template <> void EmbAJAXVariable<bool>::updateFromDriverArg(const char* argname) {
    _driver->getArg(argname, itoa_buf, sizeof(itoa_buf));
    _value = (strcmp(itoa_buf,"t") == 0);
}

template <> int EmbAJAXVariable<int>::intValue() const {
    return _value;
}

template <> int EmbAJAXVariable<char*>::intValue() const {
    return atoi(_value);
}

template <> int EmbAJAXVariable<bool>::intValue() const {
    return _value;
}

template <> bool EmbAJAXVariable<int>::boolValue() const {
    return _value == 1;
}

template <> bool EmbAJAXVariable<char*>::boolValue() const {
    return strcmp(_value,"t") == 0;
}

template <> bool EmbAJAXVariable<bool>::boolValue() const {
    return _value;
}


//////////////////////// EmbAJAXStyle /////////////////////////////

const char* EmbAJAXStyle::value(uint8_t which) const {
    if (which == EmbAJAXStyle::Style) return _style;
    return EmbAJAXMutableSpan::value(which);
}

const char* EmbAJAXStyle::valueProperty(uint8_t which) const {
    if (which == EmbAJAXStyle::Style) return "style";
    return EmbAJAXMutableSpan::valueProperty(which);
}

void EmbAJAXStyle::setStyle(const char* style) {
    _style = style;
    setChanged();
}



//////////////////////// Website Manager /////////////////////////////



// Initialize web manaber
void ICACHE_FLASH_ATTR WebsiteManager::begin() {

     // If the client requests any URI
    _server.onNotFound( 

        [&]() { 

            URL = _server.uri();
            AjaxID = _server.arg("id");
            AjaxValue = _server.arg("value");    

            if( URL.endsWith("/") ) URL += F("index.html");         // If a folder is requested, send the index file
            if( !URL.startsWith("/") ) URL =+ "/";

            // AJAX request
            if( _server.method() == HTTP_POST ) {
                for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ )
                    if( URL == webpages[i].URL ) {
                        if( AjaxID == "" ) {
                            net_status.setValue( network.getNetworkStatus() * statusFlash );
                            statusFlash = statusFlash * -1;
                        }
                        (webpages[i].handler)();
                        break;
                    }    
            }

            // Page request - send it if it exists otherwise, respond with a 404 (Not Found) error
            else if( !handleFileRequest() ) _server.send( 404, PSTR("text/html"), PSTR("404: Not Found") );
        
        }

    );

    // Start LittleFS and webserver
#ifndef WEB_FLASHFILES
    LittleFS.begin(); 
#endif
    _server.begin();

    // Initialize all page Ajax
    for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ )
        (webpages[i].init)();

}


// convert the file extension to the MIME type
String ICACHE_FLASH_ATTR WebsiteManager::getContentType(String filename) { 
    if (filename.endsWith(PSTR(".html"))) return PSTR("text/html");
    else if (filename.endsWith(PSTR(".css"))) return PSTR("text/css");
    else if (filename.endsWith(PSTR(".js"))) return PSTR("application/javascript");
    else if (filename.endsWith(PSTR(".ico"))) return PSTR("image/x-icon");
    else if (filename.endsWith(PSTR(".gif"))) return PSTR("image/gif");
    else if (filename.endsWith(PSTR(".svg"))) return PSTR("image/svg+xml");
    else if (filename.endsWith(PSTR(".jpg"))) return PSTR("image/jpeg");
    else if (filename.endsWith(PSTR(".png"))) return PSTR("image/png");
    return PSTR("text/plain");
}



// Send the right file to the client (if it exists)
// Assumes GZIPed files
bool ICACHE_FLASH_ATTR WebsiteManager::handleFileRequest() {

    String contentType = getContentType(URL);                  // Get the MIME type

#ifdef WEB_FLASHFILES                   // Are we using flash instead of LittleFS for web files
    // Try from Flash
    for( uint i=0; i<(sizeof(websiteFiles)/sizeof(t_websitefiles)); i++ ) {
        if( strcmp_P( URL.c_str(), websiteFiles[i].path ) == 0 ) {

            LOGF_HIGH( PSTR("(Website) Web server - flash: %s"), URL.c_str() );

            _server.sendHeader(PSTR("Content-Encoding"),PSTR("gzip"));
            _server.setContentLength(websiteFiles[i].len);
            _server.send(200,contentType,"");

            WiFiClient client = _server.client();
            
            client.write_P((const char*)FPSTR(websiteFiles[i].content),websiteFiles[i].len);

            return true;
        }
    }
#endif

#ifndef WEB_FLASHFILES                  // Are we using flash instead of LittleFS for web files
    // Try from LittleFS
    String path = "/www" + URL + ".gz";
    if( LittleFS.exists(path) ) {                               // If the file exists then send it

        LOGF_HIGH( PSTR("(Website) Web server - file: %s"), URL.c_str() );

        File file = LittleFS.open(path, "r");
        _server.streamFile(file, contentType);
        file.close();

        // Re-initialize Ajax on page load
        for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ )
            if( URL == webpages[i].URL ) {
                (webpages[i].init)();
                break;
            }

        return true;
    }
#endif

    LOG(PSTR("(Website) Web server - file not found"));

    return false;                                         // If the file doesn't anywhere, return false
}



// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WEBSITE_MANAGER)
    EmbAJAXVarInt net_status("net_status",0);
    WebsiteManager website;
#endif