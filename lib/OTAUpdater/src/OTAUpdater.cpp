/* Remote Updater Library

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

Implements a remote update service for ESP using GitHub to store code and
binaries built by Travis-CI.

JSON used to get versioning from GitHub is in this format:

{
    "repo": "123456789012345678901234567890",
    "releases": [{
        "tag": "xx.xx.xx",
        "date": "2019-12-08T20:58:02Z",
        "assets": [{
            "name": "12345678901234567890-Fvxx.xx.xx.bin",
            "size": 1234567,
            "URL": "https:\/\/github.com\/1234567890\/123456789012345678901234567890\/releases\/download\/1.9.15\/12345678901234567890-Fvxx.xx.xx.bin.gz"
        }, {
            "name": "12345678901234567890-Fvxx.xx.xx.bin",
            "size": 1234567,
            "URL": "https:\/\/github.com\/1234567890\/123456789012345678901234567890\/releases\/download\/1.9.15\/12345678901234567890-Fvxx.xx.xx.bin.gz"
        }, {
            "name": "12345678901234567890-Fvxx.xx.xx.bin.gz",
            "size": 1234567,
            "URL": "https:\/\/github.com\/1234567890\/123456789012345678901234567890\/releases\/download\/1.9.15\/12345678901234567890-Fvxx.xx.xx.bin.gz"
        }, {
            "name": "12345678901234567890-Fvxx.xx.xx.bin.gz",
            "size": 1234567,
            "URL": "https:\/\/github.com\/1234567890\/123456789012345678901234567890\/releases\/download\/1.9.15\/12345678901234567890-Fvxx.xx.xx.bin.gz"
        }]
    }]
}

https://arduinojson.org/v6/assistant/

*/


#include <ArduinoJson.h>

#include "Device.h"
#include "Logger.h"
#include "OTAUpdater.h"



bool OTAUpdater::_doUpdateCheck = false;


void ICACHE_FLASH_ATTR OTASettings::setDefaults() {
    strcpy_P(service, flag_UPDATER_SERVICE);
    strcpy_P(repo, flag_UPDATER_REPO);
    strcpy_P(user, flag_UPDATER_USER);
    strcpy_P(token, flag_UPDATER_TOKEN);
    interval = flag_UPDATER_INTERVAL;
    skipUpdates = flag_UPDATER_SKIP;
}




// TODO - Change like ESP8266HTTPUpdate::handleUpdate

void ICACHE_FLASH_ATTR OTAUpdater::begin( WiFiClient &client, OTASettings &settings ) {

    _client = &client;
    _settings = &settings;

    strcpy_P(_assetRequestURL, PSTR("http://"));
    strcat(_assetRequestURL, _settings->service);
    strcat_P(_assetRequestURL, PSTR("?repo="));
    strcat(_assetRequestURL, _settings->repo);
    strcat_P(_assetRequestURL, PSTR("&user="));
    strcat(_assetRequestURL, _settings->user);
    if( _settings->token[0] != '\0' ) {
        strcat_P(_assetRequestURL, PSTR("&token="));
        strcat(_assetRequestURL, _settings->token);
    }

    _doUpdateCheck = false;

    if( _updateCheck.active() ) _updateCheck.detach();
    _updateCheck.attach( _settings->interval, TriggerUpdateCheck );

    LOG(F("(Updater) Starting updater"));
}


void OTAUpdater::TriggerUpdateCheck() {
    _doUpdateCheck = true;
}

// TODO change to POST intead of GET

String ICACHE_FLASH_ATTR OTAUpdater::getLatestBuild() {

    LOG_HIGH(F("(Updater) Checking latest build..."));

    HTTPClient http;

    http.setReuse(false);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);


    LOGF_DETAIL( PSTR("(Updater) URL: %s"), _assetRequestURL );

    http.begin( *_client, _assetRequestURL );

    http.setUserAgent(F("ESP8266-http-Update"));
    http.addHeader(F("Content-Type"), F("content-type:text/plain"));

    int httperror = http.GET();
    String httppayload = http.getString();
    http.end();

    if( httperror != HTTP_CODE_OK ) {

        if( httperror < 0 ) {
           LOGF_CRITICAL( PSTR("(Updater) Error getting latest release: ERROR %s"), http.errorToString(httperror).c_str() );
        }
        else {
            LOGF_CRITICAL( PSTR("(Updater) Error getting latest release: ERROR %i"), httperror );
        }

        return "";
    }
    else {

        // Expecting JSON back with latest release details

        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(2) + 5*JSON_OBJECT_SIZE(3) + 797; 
        DynamicJsonDocument responseJSON(capacity);

        DeserializationError jsonerror = deserializeJson( responseJSON, httppayload );

        if (jsonerror) LOGF_CRITICAL( PSTR("(Updater) JSON Error: %s"), jsonerror.c_str() );

        String repoName = responseJSON[F("repo")];

       LOGF_DETAIL( PSTR("(Updater) Returned Repo: %s"), repoName.c_str() );

        if( repoName != _settings->repo ) {

            LOG_CRITICAL(F("(Updater) JSON Error getting latest release"));

            return "";
        }

        JsonObject latestRelease = responseJSON[F("releases")][0];
        const char* latestTag = latestRelease[F("tag")];
        const char* releaseDate = latestRelease[F("date")];         // TODO - releaseDate isn't used

        strcpy(_latestTag, latestTag);

        LOGF_DETAIL( PSTR("(Updater) Latest version: %s"), _latestTag );
        LOGF_DETAIL( PSTR("(Updater) Release date: %s"), releaseDate );

        return _latestTag;
    }

    return "";

}
 

#ifndef WEB_FLASHFILES      // Are we using flash instead of LittleFS for web files
HTTPUpdateResult ICACHE_FLASH_ATTR OTAUpdater::UpdateFS( const bin_type type ) {

    // Update file system
    char littleFSFileRequest[OTA_MAX_IMAGE_URL_LEN];
    strcpy(littleFSFileRequest, _assetRequestURL);
    strcat_P(littleFSFileRequest, PSTR("&asset="));
    strcat_P(littleFSFileRequest,  flag_DEVICE_CODE);
    strcat(littleFSFileRequest, _FSSuffix);
    strcat_P(littleFSFileRequest, PSTR("&tag="));
    strcat(littleFSFileRequest, _latestTag);
    strcat(littleFSFileRequest, ( type == GZ ? "&type=gz" : ""));

    LOG(F("(Updater) Updating File System"));
    LOGF_HIGH(  PSTR("(Updater) File system image request: %s"), littleFSFileRequest );

    HTTPUpdateResult ret;

    if( _settings->skipUpdates ) {

        LOG(F("(Updater) Skipping update"));
        ret = HTTP_UPDATE_NO_UPDATES;

    }
    else ret = ESPhttpUpdate.updateFS( *_client, littleFSFileRequest );

    switch(ret) {

    case HTTP_UPDATE_FAILED: {
            LOGF_CRITICAL(  PSTR("(Updater) File system update failed - Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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
#endif


HTTPUpdateResult ICACHE_FLASH_ATTR OTAUpdater::UpdateProg( const bin_type type, bool restart ) {

    // Update program image
    char imageFileRequest[OTA_MAX_IMAGE_URL_LEN];
    strcpy(imageFileRequest, _assetRequestURL);
    strcat_P(imageFileRequest, PSTR("&asset="));
    strcat_P(imageFileRequest,  flag_DEVICE_CODE);
    strcat(imageFileRequest, _progSuffix);
    strcat_P(imageFileRequest, PSTR("&tag="));
    strcat(imageFileRequest, _latestTag);
    strcat(imageFileRequest, ( type == GZ ? "&type=gz" : ""));
    
    LOG(F("(Updater) Updating Program"));
    LOGF_HIGH( PSTR("(Updater) Program image request: %s"), imageFileRequest );

    HTTPUpdateResult ret;

    if( _settings->skipUpdates ) {

        LOG(F("(Updater) Skipping update"));
        ret = HTTP_UPDATE_NO_UPDATES;

    }
    else {
        
        ESPhttpUpdate.rebootOnUpdate(false);
        ret = ESPhttpUpdate.update( *_client, imageFileRequest );
    }

    switch(ret) {

    case HTTP_UPDATE_FAILED: {
            LOGF_CRITICAL( PSTR("(Updater) Program update failed - Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str() );
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
        LOG_CRITICAL(F("(Updater) Rebooting in 5 sec"));
        delay(5000);
        ESP.restart();

    }

    return ret;

}



void OTAUpdater::handle() {

    if ( _doUpdateCheck && WiFi.status() == WL_CONNECTED ) {

        _doUpdateCheck = false;

        String currentTag = FPSTR(flag_BUILD_TAG);

        LOGF( PSTR("(Updater) Current version: %s"), currentTag.c_str() );

        // Check for update

        String checkTag = getLatestBuild();

        if( checkTag == "" ) return;

        LOGF( PSTR("(Updater) Latest version: %s"), checkTag.c_str() );

        if( checkTag == currentTag ) {
            LOG(F("(Updater) No new update"));  
            return;
        }
        
#ifndef WEB_FLASHFILES      // Are we using flash instead of LittleFS for web files
        if( UpdateFS( RAW ) == HTTP_UPDATE_OK ) UpdateProg( GZ, true );         // Compressed only works for program, not file system
#else
        UpdateProg( GZ, true );
#endif
        
    }
}



#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ESP_REMOTE_UPDATER)
    OTAUpdater updater;
#endif

