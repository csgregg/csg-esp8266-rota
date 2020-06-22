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


#include <cstring>

#include "website.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "NetworkSetttingsPage.h"

    // WiFi 1
    EmbAJAXTextInput<MAX_SSID_LEN> wifi1_ssid("wifi1_ssid");
    EmbAJAXTextInput<MAX_PASSWORD_LEN> wifi1_password("wifi1_password");
    EmbAJAXCheckButton wifi1_dhcp_mode("wifi1_dhcp_mode","");
    EmbAJAXMutableSpan wifi1_dhcp_mode_label("wifi1_dhcp_mode_label");
    EmbAJAXHideableContainer<0> wifi1_static_show("wifi1_static_show",NULL);
    EmbAJAXTextInput<15> wifi1_ip("wifi1_ip");
    EmbAJAXTextInput<15> wifi1_subnet("wifi1_subnet");
    EmbAJAXTextInput<15> wifi1_gateway("wifi1_gateway");
    EmbAJAXTextInput<15> wifi1_dns1("wifi1_dns1");
    EmbAJAXTextInput<15> wifi1_dns2("wifi1_dns2");


    MAKE_EmbAJAXPage( ajax, "", "",

        // WiFi 1
        &wifi1_ssid,
        &wifi1_password,
        &wifi1_dhcp_mode,
        &wifi1_dhcp_mode_label,
        &wifi1_static_show,
        &wifi1_ip,
        &wifi1_subnet,
        &wifi1_gateway,
        &wifi1_dns1,
        &wifi1_dns2

    )



    
void initAjax() {

  //  if( initialized ) return;
  //  initialized = true;

    StationConfig wifiStation;
    char ipbuffer[15];
    bool dhcp;

    DEBUG("Initialize AJAX elements");


    // WiFi 1
    // ======

    wifiStation = config.settings.networkConfig.stationSettings[0];

    wifi1_ssid.setValue(wifiStation.SSID);
    wifi1_password.setValue(wifiStation.password);

    // IP Addresses
    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.ip[0], wifiStation.ip[1], wifiStation.ip[2], wifiStation.ip[3] );
    wifi1_ip.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.subnet[0], wifiStation.subnet[1], wifiStation.subnet[2], wifiStation.subnet[3] );
    wifi1_subnet.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.gateway[0], wifiStation.gateway[1], wifiStation.gateway[2], wifiStation.gateway[3] );
    wifi1_gateway.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns1[0], wifiStation.dns1[1], wifiStation.dns1[2], wifiStation.dns1[3] );
    wifi1_dns1.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns2[0], wifiStation.dns2[1], wifiStation.dns2[2], wifiStation.dns2[3] );
    wifi1_dns2.setValue(ipbuffer);

    // DHCP Mode
    dhcp = (wifiStation.DHCPMode == DHCP);
    wifi1_dhcp_mode.setChecked(dhcp);
    wifi1_dhcp_mode_label.setValue(dhcp ? "Dynamic IP" : "Static IP");
    wifi1_static_show.setVisible(!dhcp);



}

void handleAjax() {

    StationConfig wifiStation;
    bool dhcp;

    DEBUG("Handle AJAX");


    // WiFi 1
    // ======

    wifiStation = config.settings.networkConfig.stationSettings[0];

    strncpy(wifiStation.SSID, wifi1_ssid.value(), MAX_SSID_LEN);
    strncpy(wifiStation.password, wifi1_password.value(), MAX_PASSWORD_LEN);
    
    wifiStation.ip.fromString(wifi1_ip.value());

    wifiStation.subnet.fromString(wifi1_subnet.value());
    wifiStation.gateway.fromString(wifi1_gateway.value());
    wifiStation.dns1.fromString(wifi1_dns2.value());
    wifiStation.dns2.fromString(wifi1_dns2.value());

    // DHCP Mode
    dhcp = wifi1_dhcp_mode.isChecked();
    wifiStation.DHCPMode = (dhcp ? DHCP : STATIC);
    wifi1_dhcp_mode_label.setValue(dhcp ? "Dynamic IP" : "Static IP");
    wifi1_static_show.setVisible(!dhcp);


}



// Webpages
// ========
/*
// Page handlers
PageHandler pagehandlers[] = {
    {networksettingsajax.URL, networksettingsajax.handler, networksettingsajax.init}
};

*/

// Website Manager
// ===============
/*
// Call appropriate page handler
void WebsiteManager::handleAJAX( const String path ) {

    for( u_int i = 0; i < sizeof(pagehandlers)/sizeof(PageHandler); i++ )
        if( path == pagehandlers[i].URL ) {
            (pagehandlers[i].handler)();
            return;
        }


}
*/
/*
// Call appropriate page initializer
void WebsiteManager::handleInit( const String path ) {

    for( u_int i = 0; i < sizeof(pagehandlers)/sizeof(PageHandler); i++ )
        if( path == pagehandlers[i].URL ) {
            (pagehandlers[i].init)();
            return;
        }
}
*/

// Initialize web manaber
void WebsiteManager::begin() {

     // If the client requests any URI
    _server.onNotFound( 

        [&]() {                             

            // AJAX request
            if( _server.method() == HTTP_POST ) 
                // handleAJAX( _server.uri() ); 
                ajax.handleRequest( handleAjax );

            // Page request - send it if it exists otherwise, respond with a 404 (Not Found) error
            else if( !handleSPIFFS( _server.uri() ) ) _server.send( 404, F("text/html"), F("404: Not Found") );
        
        }

        

    );


    // Start SPIFFS and webserver
    SPIFFS.begin(); 
    _server.begin();
/*
    for( u_int i = 0; i < sizeof(pagehandlers)/sizeof(PageHandler); i++ ) {
        (pagehandlers[i].handler)();

    }
*/

 /*               ajax.handleRequest( []() {
                    handleAjax();
                    } );


*/
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
    //    handleInit(shortpath);
    initAjax();

        return true;
    }

    LOG(F("(Network) Web server - file not found"));

    return false;                                         // If the file doesn't exist, return false
}



// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WEBSITE_MANAGER)
    WebsiteManager website;
#endif