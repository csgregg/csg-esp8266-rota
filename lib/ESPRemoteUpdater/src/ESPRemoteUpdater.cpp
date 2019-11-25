/* Remote Updater Library

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

*/


#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>

#include "IOTDevice.h"
#include "Logger.h"
#include "ESPRemoteUpdater.h"



ESPRemoteUpdater::ESPRemoteUpdater() {

}

bool ESPRemoteUpdater::_doUpdateCheck;


void ESPRemoteUpdater::setup( const String &assetRequestURL, const String &deviceCode, const String &buildTag, float updateinterval ) {

    _assetRequestURL = assetRequestURL;
    _deviceCode = deviceCode;
    _buildTag = buildTag;
    _updateinterval = updateinterval;

}



void ESPRemoteUpdater::begin( HTTPClient &http, WiFiClient &client ) {

    _http = &http;
    _client = &client;

    _doUpdateCheck = false;

    _updateCheck.attach( _updateinterval, TriggerUpdateCheck );
}


void ESPRemoteUpdater::TriggerUpdateCheck() {
    _doUpdateCheck = true;
}


bool ESPRemoteUpdater::getLatestBuild() {

    LOG("Updating Firmware...");

    if ( WiFi.status() == WL_CONNECTED && _http != NULL ) {

        _http->begin( *_client, _assetRequestURL );
        int httpCode = _http->GET();

        if( httpCode != HTTP_CODE_OK ) {
            _http->end();
            DEBUG("Error getting latest release - Error: " + String(httpCode));

            return false;
        }
        else {

            // NEED TO CHANGE 
            // For stuations like a captive portal redirect which returns an alternative page redirect. The page content gets loaded as the latest tag which is bad
            // Suggest - githubfetchasset needs to return JSON which can then be confirmed and interpreted. 

            _latestTag = _http->getString();
            _http->end();
            
            LOG("Lastest version: " + _latestTag);

            return true;
        }
    }
    else{
         DEBUG("Cannot connect to update service");
    }

    return false;
}
 

void ESPRemoteUpdater::handle() {

    if ( WiFi.status() == WL_CONNECTED && _http != NULL && _doUpdateCheck ) {

        _doUpdateCheck = false;

        getLatestBuild();
    }
}



#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ESP_REMOTE_UPDATER)
    ESPRemoteUpdater updater;
#endif

