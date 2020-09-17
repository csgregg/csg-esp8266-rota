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


*/


#include <LittleFS.h>

#include "WebManager.h"
#include "Logger.h"

// TODO - Do we want to move the web pages into separate lib?
#include "NetworkSetttingsPage.h"
#include "AboutPage.h"
#include "IndexPage.h"
#include "SystemPage.h"

    
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
void WebsiteManager::begin() {

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
                        int_status.setValue(network.isInternetConnected());
                        (webpages[i].handler)();
                        break;
                    }    
            }

            // Page request - send it if it exists otherwise, respond with a 404 (Not Found) error
            else if( !handlelittleFS() ) _server.send( 404, F("text/html"), F("404: Not Found") );
        
        }

    );

    // Start LittleFS and webserver
    LittleFS.begin(); 
    _server.begin();

    // Initialize all page Ajax
    for( u_int i = 0; i < sizeof(webpages)/sizeof(PageHandler); i++ )
        (webpages[i].init)();

}


// convert the file extension to the MIME type
String WebsiteManager::getContentType(String filename) { 
    if (filename.endsWith(F(".html.gz"))) return F("text/html");
    else if (filename.endsWith(F(".html"))) return F("text/html");
    else if (filename.endsWith(F(".css.gz"))) return F("text/css");
    else if (filename.endsWith(F(".css"))) return F("text/css");
    else if (filename.endsWith(F(".js.gz"))) return F("application/javascript");
    else if (filename.endsWith(F(".js"))) return F("application/javascript");
    else if (filename.endsWith(F(".ico.gz"))) return F("image/x-icon");
    else if (filename.endsWith(F(".ico"))) return F("image/x-icon");
    else if (filename.endsWith(F(".woff.gz"))) return F("application/font-woff");
    else if (filename.endsWith(F(".woff"))) return F("application/font-woff");
    return F("text/plain");
}



// Send the right file to the client (if it exists)
bool WebsiteManager::handlelittleFS() {

    String shortpath = URL;

    logger.printf( LOG_HIGH, TAG_STATUS, "(Website) Web server - file: %s", shortpath.c_str() );

    String path = "/www" + shortpath + ".gz";

    String contentType = getContentType(path);              // Get the MIME type
    if( LittleFS.exists(path) ) {                             // If the file exists then send it
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

    LOG(F("(Website) Web server - file not found"));

    return false;                                         // If the file doesn't exist, return false
}



// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WEBSITE_MANAGER)
    EmbAJAXVarBool int_status("int_status");
    WebsiteManager website;
#endif