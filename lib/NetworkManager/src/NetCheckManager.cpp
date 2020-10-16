/* Internet Check Manager Library

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

Manages Internet Checking Functions

*/

#include <ESP8266HTTPClient.h>

#include "NetCheckManager.h"
#include "Logger.h"
#include "Device.h"




void ICACHE_FLASH_ATTR NetCheckSettings::setDefaults() {
    mode = flag_NET_CHECKER;
    strcpy_P( checkService, flag_NET_CHECK_SERVICE );
    interval = flag_NET_CHECK_INTERVAL;
}



// Network Manager Class


bool NetCheckManager::_doNetCheck = false;



void ICACHE_FLASH_ATTR NetCheckManager::begin(WiFiClient &client, NetCheckSettings &settings) {

    _settings = &settings;
    _client = &client;

    _doNetCheck = _settings->mode;
    _ConnectedToInternet = false;

    if( _netCheck.active() ) _netCheck.detach();

    if( _settings->mode ) _netCheck.attach( _settings->interval, TriggerNetCheck ); 

};


void NetCheckManager::handle() {

    if ( _doNetCheck ) {

        _doNetCheck = false;
        _ConnectedToInternet = false;

        if( WiFi.status() == WL_CONNECTED ) {

            HTTPClient http;

            http.setReuse(false);
            http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

            LOG_HIGH(PSTR("(Network) Checking for internet"));

            char url[NETCHECK_MAX_SERVICE_LEN+sizeof("http://")];
            strcpy_P(url,PSTR("http://"));
            strncat(url, _settings->checkService,sizeof(url));

            http.begin( *_client, url );

            http.setUserAgent(FPSTR(flag_DEVICE_CODE));
            http.addHeader(PSTR("Content-Type"), PSTR("text/plain"));

            int httpresponse = http.GET();

            http.end();

            _ConnectedToInternet = ( httpresponse == HTTP_CODE_NO_CONTENT );
        }

        LOG_HIGH( _ConnectedToInternet ? PSTR("(Network) Connected") : PSTR("(Network) Not connected") );

    }
}
