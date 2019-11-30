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
#include <ArduinoJson.h>

#include "Logger.h"
#include "ESPRemoteUpdater.h"



ESPRemoteUpdater::ESPRemoteUpdater() {

}

bool ESPRemoteUpdater::_doUpdateCheck;


void ESPRemoteUpdater::setup( const String &assetRequestURL, const String &deviceCode, const String &buildTag, long updateinterval, bool skip = false ) {

    _assetRequestURL = assetRequestURL;
    _deviceCode = deviceCode;
    _buildTag = buildTag;
    _updateinterval = updateinterval;
    _skipUpdates = skip;

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


String ESPRemoteUpdater::getLatestBuild() {

    LOG("Checking latest build...");

    if ( _http != NULL ) {

        DEBUG("Update URL: " + _assetRequestURL);

        _http->begin( *_client, _assetRequestURL );

        _lastError = _http->GET();

        if( _lastError != HTTP_CODE_OK ) {

            _http->end();

            DEBUG("Error getting latest release - Error: " + _http->errorToString(_lastError));

            return "";
        }
        else {

            // Expecting JSON back with latest release details

            const size_t capacity = 600;
            DynamicJsonDocument responseJSON(capacity);

            String rawJSON = _http->getString();
            _http->end();

            DEBUG("JSON: " + rawJSON);

            deserializeJson( responseJSON, rawJSON );

            String repoName = responseJSON["repo"];


            // TODO: Better error handling

            if( repoName != device.repoName ) {

                DEBUG("JSON Error getting latest release");

                return "";
            }

            JsonObject latestRelease = responseJSON["releases"][0];
            const char* latestTag = latestRelease["tag"];
            const char* releaseDate = latestRelease["date"];

            _latestTag = latestTag;
            String str = releaseDate;
            
            LOG("Latest version: " + _latestTag);
            LOG("Release date: " + str);

            return _latestTag;
        }
    }
    else {

        _lastError = HTTP_CODE_BAD_REQUEST;
        DEBUG("Cannot connect to update service");
    }

    return "";

}
 


HTTPUpdateResult ESPRemoteUpdater::UpdateFS() {

    // Update SPIFFS file system
    String spiffsFileRequest = _assetRequestURL + "&asset=" + _deviceCode + _FSSuffix + "&tag=" + _latestTag;
    LOG("File system image request: " + spiffsFileRequest);

    HTTPUpdateResult ret;

    if( _skipUpdates ) {

        LOG("Skipping update");
        ret = HTTP_UPDATE_NO_UPDATES;

    }
    else ret = ESPhttpUpdate.updateSpiffs( *_client, spiffsFileRequest );

    switch(ret) {

    case HTTP_UPDATE_FAILED:
        logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);
        logger.printf("File system update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        LOG("No new file system update");
        break;
        
    case HTTP_UPDATE_OK:
        LOG("File system updated successfully");
        break;
    }

    return ret;

}



HTTPUpdateResult ESPRemoteUpdater::UpdateProg( bool restart = false ) {

    // Update program image
    String imageFileRequest = _assetRequestURL + "&asset=" + _deviceCode + _progSuffix + "&tag=" + _latestTag;
    LOG("Program image request: " + imageFileRequest);

    HTTPUpdateResult ret;

    if( _skipUpdates ) {

        LOG("Skipping update");
        ret = HTTP_UPDATE_NO_UPDATES;

    }
    else {
        
        ESPhttpUpdate.rebootOnUpdate(false);
        ret = ESPhttpUpdate.update( *_client, imageFileRequest );
    }

    switch(ret) {

    case HTTP_UPDATE_FAILED:
        logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);
        logger.printf("Program update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        LOG("No new program update");
        break;
        
    case HTTP_UPDATE_OK:
        LOG("Program updated successfully");
        break;
    }

    if( ret == HTTP_UPDATE_OK && restart ) {
        
        logger.println(LOG_CRITICAL, TAG_STATUS, "Rebooting in 5 sec");
        delay(5000);
        ESP.restart();

    }

    return ret;

}



void ESPRemoteUpdater::handle() {

    if ( WiFi.status() == WL_CONNECTED && _http != NULL && _doUpdateCheck ) {

        _doUpdateCheck = false;

        LOG("Current version: " + _buildTag);

        // Check for update
        if( getLatestBuild() == _buildTag ){

           LOG("No new update");      

        }
        else if( UpdateFS() == HTTP_UPDATE_OK ) UpdateProg( true );
        
    }
}



#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ESP_REMOTE_UPDATER)
    ESPRemoteUpdater updater;
#endif

