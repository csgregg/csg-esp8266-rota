/**
 * @file        NetCheckManager.cpp
 * @author      Chris Gregg
 * 
 * @brief       Manages Internet Checking Functions
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
#include <ESP8266HTTPClient.h>

// Project Libraries
#include "NetCheckManager.h"
#include "Logger.h"
#include "IOTDevice.h"



////////////////////////////////////////////
//// NetCheckSettings Class

// Public:

// Resets checker settings to defaults
void ICACHE_FLASH_ATTR NetCheckSettings::setDefaults() {
    enabled = flag_NET_CHECKER;
    strcpy_P( checkService, flag_NET_CHECK_SERVICE );
    interval = flag_NET_CHECK_INTERVAL;
}


////////////////////////////////////////////
//// Network Manager Class

// Public:

// Starts the internet checked service
void ICACHE_FLASH_ATTR NetCheckManager::Begin (WiFiClient& client, NetCheckSettings& settings ) {

    _settings = &settings;
    _client = &client;

    _doNetCheck = _settings->enabled;
    _isConnectedToInternet = false;

    if( _netCheck.active() ) _netCheck.detach();

    if( _settings->enabled ) _netCheck.attach( _settings->interval, TriggerNetCheck ); 

};


// Handles repeated checking tasks
void NetCheckManager::Handle() {

    if ( _doNetCheck ) {

        _doNetCheck = false;
        _isConnectedToInternet = false;

        if( WiFi.status() == WL_CONNECTED ) {

            // Start conection
            HTTPClient http;
            http.setReuse( false );
            http.setFollowRedirects( HTTPC_FORCE_FOLLOW_REDIRECTS );

            LOG_HIGH( PSTR("(Network) Checking for internet") );

            // Build URL
            char url[NETCHECK_MAX_SERVICE_LEN+sizeof("http://")];       // TODO - move this to member
            strcpy_P( url, PSTR("http://") );
            strncat( url, _settings->checkService, sizeof(url) );

            // Connect
            http.begin( *_client, url );
            http.setUserAgent( FPSTR(flag_DEVICE_CODE) );
            http.addHeader( PSTR("Content-Type"), PSTR("text/plain") );
            int httpresponse = http.GET();
            http.end();

            // Confirm expected result
            _isConnectedToInternet = ( httpresponse == HTTP_CODE_NO_CONTENT );
        }

        LOG_HIGH( _isConnectedToInternet ? PSTR("(Network) Connected") : PSTR("(Network) Not connected") );

    }
}


// Protected:

bool NetCheckManager::_doNetCheck = false;      // Initialize static member


