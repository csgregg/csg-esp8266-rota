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



        
{
    "repo": "123456789012345678901234567890",
    "releases": [{
        "tag": "xx.xx.xx",
        "date": "2019-12-08T20:58:02Z",
        "assets": [{
            "name": "12345678901234567890-Fvxx.xx.xx.bin",
            "size": 0000000,
            "URL": "https:\/\/github.com\/1234567890\/123456789012345678901234567890\/releases\/download\/1.9.15\/12345678901234567890-Fvxx.xx.xx.bin"
        }, {
            "name": "12345678901234567890-Fvxx.xx.xx.bin",
            "size": 0000000,
            "URL": "https:\/\/github.com\/1234567890\/123456789012345678901234567890\/releases\/download\/1.9.15\/12345678901234567890-Fvxx.xx.xx.bin"
        }]
    }]
}


https://arduinojson.org/v6/assistant/

        

*/


#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


#include "Logger.h"
#include "ESPRemoteUpdater.h"



bool ESPRemoteUpdater::_doUpdateCheck;


void ICACHE_FLASH_ATTR ESPRemoteUpdater::setup( const String &service, const String &repo, const String &user, const String &token, const String &deviceCode, const String &buildTag, long updateinterval, bool skip = false ) {

    _assetRequestURL = PSTR("http://") + service + PSTR("?repo=") + repo + PSTR("&user=") + user;
    if( token != "" ) _assetRequestURL += PSTR("&token=") + token;

    _repoName = repo;
    _deviceCode = deviceCode;
    _buildTag = buildTag;
    _updateinterval = updateinterval;
    _skipUpdates = skip;


}



// TODO - Change like ESP8266HTTPUpdate::handleUpdate

void ICACHE_FLASH_ATTR ESPRemoteUpdater::begin( WiFiClient &client ) {

    _client = &client;

    _doUpdateCheck = false;

    _updateCheck.attach( _updateinterval, TriggerUpdateCheck );

    LOG(F("(Updater) Starting updater"));
}


void ESPRemoteUpdater::TriggerUpdateCheck() {
    _doUpdateCheck = true;
}

// TODO change to POST intead of GET

String ICACHE_FLASH_ATTR ESPRemoteUpdater::getLatestBuild() {

    LOG(F("(Updater) Checking latest build..."));

    HTTPClient http;

    http.setReuse(false);

    logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
    PGM_P format1 = PSTR("(Updater) URL: %s");
    logger.printf( format1, _assetRequestURL.c_str() );

    http.begin( *_client, _assetRequestURL );

    http.setUserAgent(F("ESP8266-http-Update"));
    http.addHeader(F("Content-Type"), F("content-type:text/plain"));

    int httperror = http.GET();
    String httppayload = http.getString();
    http.end();

    if( httperror != HTTP_CODE_OK ) {

        logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);

        if( httperror < 0 ) {
            PGM_P format2 = PSTR("(Updater) Error getting latest release: ERROR %s");
            logger.printf( format2, http.errorToString(httperror).c_str() );
        }
        else {
            PGM_P format3 = PSTR("(Updater) Error getting latest release: ERROR %i");
            logger.printf( format3, httperror );
        }

        return "";      // TODO - should this be String();
    }
    else {

        // Expecting JSON back with latest release details


        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + 435; 
        DynamicJsonDocument responseJSON(capacity);
        
        DeserializationError jsonerror = deserializeJson( responseJSON, httppayload );

        if (jsonerror) LOG(jsonerror.c_str());          // TODO better error handling

        String repoName = responseJSON[F("repo")];

        logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
        PGM_P format4 = PSTR("(Updater) Returned Repo: %s");
        logger.printf( format4, repoName.c_str() );

        if( repoName != _repoName ) {

            LOG_CRITICAL(F("(Updater) JSON Error getting latest release"));

            return "";
        }

        JsonObject latestRelease = responseJSON[F("releases")][0];
        const char* latestTag = latestRelease[F("tag")];
        const char* releaseDate = latestRelease[F("date")];

        _latestTag = latestTag;
        String str = releaseDate;
        
        logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
        PGM_P format5 = PSTR("(Updater) Latest version: %s");
        logger.printf( format5, _latestTag.c_str() );
        PGM_P format6 = PSTR("(Updater) Release date: %s");
        logger.printf( format6, str.c_str() );

        return _latestTag;
    }

    return "";

}
 


HTTPUpdateResult ICACHE_FLASH_ATTR ESPRemoteUpdater::UpdateFS() {

    // Update SPIFFS file system
    String spiffsFileRequest = _assetRequestURL + PSTR("&asset=") + _deviceCode + _FSSuffix + PSTR("&tag=") + _latestTag;

    LOG(F("(Updater) Updating File System"));
    logger.setTypeTag(LOG_DETAIL,TAG_STATUS);
    PGM_P format1 = PSTR("(Updater) File system image request: %s");
    logger.printf( format1, spiffsFileRequest.c_str() );

    HTTPUpdateResult ret;

    if( _skipUpdates ) {

        LOG(F("(Updater) Skipping update"));
        ret = HTTP_UPDATE_NO_UPDATES;

    }
    else ret = ESPhttpUpdate.updateSpiffs( *_client, spiffsFileRequest );

    switch(ret) {

    case HTTP_UPDATE_FAILED: {
            logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);
            PGM_P format2 = PSTR("(Updater) File system update failed - Error (%d): %s");
            logger.printf( format2, ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        }
        break;

    case HTTP_UPDATE_NO_UPDATES:
        LOG(F("(Updater) No new file system update"));
        break;
        
    case HTTP_UPDATE_OK:
        LOG(F("(Updater) File system updated successfully"));
        break;
    }

    return ret;

}



HTTPUpdateResult ICACHE_FLASH_ATTR ESPRemoteUpdater::UpdateProg( bool restart = false ) {

    // Update program image
    String imageFileRequest = _assetRequestURL + PSTR("&asset=") + _deviceCode + _progSuffix + PSTR("&tag=") + _latestTag;

    LOG(F("(Updater) Updating Program"));
    logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
    PGM_P format1 = PSTR("(Updater) Program image request: %s");
    logger.printf( format1, imageFileRequest.c_str() );

    HTTPUpdateResult ret;

    if( _skipUpdates ) {

        LOG(F("(Updater) Skipping update"));
        ret = HTTP_UPDATE_NO_UPDATES;

    }
    else {
        
        ESPhttpUpdate.rebootOnUpdate(false);
        ret = ESPhttpUpdate.update( *_client, imageFileRequest );
    }

    switch(ret) {

    case HTTP_UPDATE_FAILED: {
            logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);
            PGM_P format2 = PSTR("(Updater) Program update failed - Error (%d): %s");
            logger.printf( format2, ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str() );
        }
        break;

    case HTTP_UPDATE_NO_UPDATES:
        LOG(F("(Updater) No new program update"));
        break;
        
    case HTTP_UPDATE_OK:
        LOG(F("(Updater) Program updated successfully"));
        break;
    }

    if( ret == HTTP_UPDATE_OK && restart ) {
        logger.println(LOG_CRITICAL, TAG_STATUS, F("(Updater) Rebooting in 5 sec"));
        delay(5000);
        ESP.restart();

    }

    return ret;

}



void ESPRemoteUpdater::handle() {

    if ( WiFi.status() == WL_CONNECTED && _doUpdateCheck ) {

        _doUpdateCheck = false;

        logger.setTypeTag(LOG_NORMAL, TAG_STATUS);
        PGM_P format1 = PSTR("(Updater) Current version: %s");
        logger.printf( format1, _buildTag.c_str() );

        // Check for update

        String checkTag = getLatestBuild();

        if( checkTag == _buildTag ) {
            LOG(F("(Updater) No new update"));  
            return;
        }

        if( checkTag == "" ) return;
        
        if( UpdateFS() == HTTP_UPDATE_OK ) UpdateProg( true );
        
    }
}



#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ESP_REMOTE_UPDATER)
    ESPRemoteUpdater updater;
#endif

