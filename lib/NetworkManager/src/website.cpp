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


#include "website.h"
#include "Logger.h"
#include "ConfigManager.h"


// Webpages
// ========

struct page_settings_wifi {

    const char* URL;
    void (*handler)();
    void (*init)();

    EmbAJAXCheckButton wifi1_dhcp_mode;
    EmbAJAXMutableSpan wifi1_dhcp_mode_label;
    EmbAJAXHideableContainer<0> wifi1_static_show;

    EmbAJAXBase* page_elements[3] = {
        &wifi1_dhcp_mode,
        &wifi1_dhcp_mode_label,
        &wifi1_static_show
    };

    page_settings_wifi( const char* pURL, void(*phandler)(), void(*pinit)() ) : 
        wifi1_dhcp_mode("wifi1_dhcp_mode", ""),
        wifi1_dhcp_mode_label("wifi1_dhcp_mode_label"),
        wifi1_static_show("wifi1_static_show",NULL),
        ajax(page_elements, "")
        {
            URL = pURL;
            handler = phandler;
            init = pinit;
        };

    EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

} page_settings_wifi("/settings_wifi.html",
    []() {  // Handler
        page_settings_wifi.ajax.handleRequest( []() {

            // WiFi 1

            // DHCP Mode
            bool dhcp1 = page_settings_wifi.wifi1_dhcp_mode.isChecked();
            config.settings.networkConfig.stationSettings->DHCPMode = (dhcp1 ? DHCP : STATIC);
            page_settings_wifi.wifi1_dhcp_mode_label.setValue(dhcp1 ? "Dynamic IP" : "Static IP");
            page_settings_wifi.wifi1_static_show.setVisible(!dhcp1);

        } );
    },
    []() {  // Initializer

        // WiFi 1

        // DHCP Mode
        bool dhcp1 = (config.settings.networkConfig.stationSettings->DHCPMode == DHCP);
        page_settings_wifi.wifi1_dhcp_mode.setChecked(dhcp1);
        page_settings_wifi.wifi1_dhcp_mode_label.setValue(dhcp1 ? "Dynamic IP" : "Static IP");
        page_settings_wifi.wifi1_static_show.setVisible(!dhcp1);

    }
);
    

struct thatwebpage {

    const char* URL;
    void (*handler)();
    void (*init)();

    EmbAJAXCheckButton check;
    EmbAJAXMutableSpan check_d;

    EmbAJAXBase* page_elements[2] = {
        &check, 
        &check_d
    };

    thatwebpage( const char* pURL, void(*phandler)(), void(*pinit)() ) : 
        check("check", ""),
        check_d("check_d"),
        ajax(page_elements, "")
        {
            URL = pURL;
            handler = phandler;
            init = pinit;
        };

    EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

} _thatpage("/that.html",
    []() {  // Hanlder
        _thatpage.ajax.handleRequest( []() {
            _thatpage.check_d.setValue(_thatpage.check.isChecked() ? " Checked" : " Not checked");
        } );
    },
    []() {  // Initializer

    }
);


// Page handlers
PageHandler pagehandlers[] = {
    {page_settings_wifi.URL, page_settings_wifi.handler, page_settings_wifi.init},
    {_thatpage.URL, _thatpage.handler, _thatpage.init},
};


// Call appropriate page handler
void WebsiteManager::handleAJAX( const String path ) {

    for( u_int i = 0; i < sizeof(pagehandlers)/sizeof(PageHandler); i++ )
        if( path == pagehandlers[i].URL ) {
            (pagehandlers[i].handler)();
            return;
        }
}


// Call appropriate page initializer
void WebsiteManager::handleInit( const String path ) {

    for( u_int i = 0; i < sizeof(pagehandlers)/sizeof(PageHandler); i++ )
        if( path == pagehandlers[i].URL ) {
            (pagehandlers[i].init)();
            return;
        }
}


// Initialize web manaber
void WebsiteManager::begin() {

     // If the client requests any URI
    _server.onNotFound( 

        [&]() {                             

            // AJAX request
            if( _server.method() == HTTP_POST ) handleAJAX( _server.uri() ); 

            // Page request - send it if it exists otherwise, respond with a 404 (Not Found) error
            else if( !handleSPIFFS( _server.uri() ) ) _server.send( 404, F("text/html"), F("404: Not Found") );
        
        }

    );

    // Start SPIFFS and webserver
    SPIFFS.begin(); 
    _server.begin();

    for( u_int i = 0; i < sizeof(pagehandlers)/sizeof(PageHandler); i++ ) (pagehandlers[i].handler)();

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
bool WebsiteManager::handleSPIFFS(String shortpath) {

    logger.setTypeTag( LOG_NORMAL, TAG_STATUS );
    logger.printf("(Network) Web server - file: %s", shortpath.c_str() );

    if (shortpath.endsWith("/")) shortpath += F("index.html");         // If a folder is requested, send the index file

    shortpath.replace(F("fonts/glyphicons-halflings-regular"), F("font"));      // Deal with std bootstrap naming

    String path = shortpath + ".gz";

    String contentType = getContentType(path);              // Get the MIME type
    if( SPIFFS.exists(path) ) {                             // If the file exists then send it
        File file = SPIFFS.open(path, "r");
        _server.streamFile(file, contentType);
        file.close();

        // Check to see if the page needs to be initialized
        handleInit(shortpath);

        return true;
    }

    LOG(F("(Network) Web server - file not found"));

    return false;                                         // If the file doesn't exist, return false
}



// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WEBSITE_MANAGER)
    WebsiteManager website;
#endif