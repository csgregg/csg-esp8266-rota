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

#define OTA_CHECK_JSON_RESPONSE_SIZE (JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(2) + 5*JSON_OBJECT_SIZE(3) + 797)



#include <ArduinoJson.h>

#include "Device.h"
#include "Logger.h"
#include "OTAUpdater.h"
#include "NetworkManager.h"



bool OTAUpdater::_doUpdateCheck = false;


void ICACHE_FLASH_ATTR OTASettings::setDefaults() {
    mode = true;
    strcpy_P(service, flag_UPDATER_SERVICE);
    strcpy_P(repo, flag_UPDATER_REPO);
    strcpy_P(user, flag_UPDATER_USER);
    strcpy_P(token, flag_UPDATER_TOKEN);
    interval = flag_UPDATER_INTERVAL;
    skipUpdates = flag_UPDATER_SKIP;
}



void ICACHE_FLASH_ATTR OTAUpdater::begin( WiFiClient &client, OTASettings &settings ) {
    _client = &client;
    begin(settings);
}
void ICACHE_FLASH_ATTR OTAUpdater::begin( OTASettings &settings ) {

    _settings = &settings;

    _doUpdateCheck = false;

    if( _updateCheck.active() ) _updateCheck.detach();

    if( _settings->mode ) {

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

        LOG(PSTR("(Updater) Starting auto updater"));

        _updateCheck.attach( _settings->interval, TriggerUpdateCheck );
    }

}


void OTAUpdater::TriggerUpdateCheck() {
    _doUpdateCheck = true;
}



bool ICACHE_FLASH_ATTR OTAUpdater::checkForUpdate() {

    LOG(PSTR("(Updater) Checking latest build"));

    LOGF_HIGH( PSTR("(Updater) URL: %s"), _assetRequestURL );

    HTTPClient http;

    http.setReuse(false);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setUserAgent(FPSTR(flag_DEVICE_CODE));
    http.useHTTP10(true);
    
    http.begin( *_client, _assetRequestURL );

    int httpcode = http.GET();

    if( httpcode != HTTP_CODE_OK ) {
        if( httpcode < 0 ) LOGF_CRITICAL( PSTR("(Updater) Error getting latest release: ERROR %s"), http.errorToString(httpcode).c_str() );
        else LOGF_CRITICAL( PSTR("(Updater) Error getting latest release: ERROR %i"), httpcode );
        return false;
    }

    // Expecting JSON back with latest release details

    DynamicJsonDocument responseJSON(OTA_CHECK_JSON_RESPONSE_SIZE);

    DeserializationError jsonerror = deserializeJson( responseJSON, http.getStream() );
    http.end();

    if (jsonerror) {
        LOGF_CRITICAL( PSTR("(Updater) JSON Error: %s"), jsonerror.c_str() );
        return false;
    }

    const char* repoName = responseJSON[F("repo")];

    LOGF_HIGH( PSTR("(Updater) Returned Repo: %s"), repoName );

    if( strcmp( repoName, _settings->repo) != 0 ) {
        LOG_CRITICAL(PSTR("(Updater) JSON Error getting latest release"));
        return false;
    }

    JsonObject latestRelease = responseJSON[F("releases")][0];
    char currentTag[OTA_MAX_TAG_LEN];
    strcpy_P(currentTag,flag_BUILD_TAG);
    const char* latestTag = latestRelease[F("tag")];
    const char* releaseDate = latestRelease[F("date")];

    if( strcmp(latestTag,"") == 0 ) {
        LOG_CRITICAL(PSTR("(Updater) Error getting latest tag"));
        return false;
    }

    LOGF_HIGH( PSTR("(Updater) Current version: %s"), currentTag );
    LOGF_HIGH( PSTR("(Updater) Latest version: %s"), latestTag );
    LOGF_HIGH( PSTR("(Updater) Release date: %s"), releaseDate );

    // Check for update
    if( strcmp( currentTag, latestTag ) == 0 ) {
        LOG_HIGH(PSTR("(Updater) No new update"));  
        return false;
    }

    strcpy(_latestTag, latestTag);
    strcpy(_latestReleaseDate, releaseDate);

    return true;

}
 

#ifndef WEB_FLASHFILES      // Are we using flash instead of LittleFS for web files
t_update_result ICACHE_FLASH_ATTR OTAUpdater::UpdateFS( const bin_type type ) {

    // Update file system
    char littleFSFileRequest[OTA_MAX_IMAGE_URL_LEN];
    strcpy(littleFSFileRequest, _assetRequestURL);
    strcat_P(littleFSFileRequest, PSTR("&asset="));
    strcat_P(littleFSFileRequest,  flag_DEVICE_CODE);
    strcat(littleFSFileRequest, _FSSuffix);
    strcat_P(littleFSFileRequest, PSTR("&tag="));
    strcat(littleFSFileRequest, _latestTag);
    strcat_P(littleFSFileRequest, (type == GZ ? PSTR("&type=gz") : PSTR("")) );

    LOG(PSTR("(Updater) Updating File System"));
    LOGF_HIGH(  PSTR("(Updater) File system image request: %s"), littleFSFileRequest );

    if( _settings->skipUpdates ) {

        LOG(PSTR("(Updater) Skipping update"));
        return UPDATE_SKIPPED;
    }
    else {
        
        HTTPUpdateResult ret;
        ret = ESPhttpUpdate.updateFS( *_client, littleFSFileRequest );

        switch(ret) {

            case HTTP_UPDATE_FAILED:
                LOGF_CRITICAL(  PSTR("(Updater) File system update failed - Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                return FS_UPDATE_FAILED;

            case HTTP_UPDATE_NO_UPDATES:
                LOG(PSTR("(Updater) No new file system update"));
                return NO_UPDATES;
                
            default:
                LOG(PSTR("(Updater) File system updated successfully"));

        }

        return UPDATE_OK;
    }
}
#endif


t_update_result ICACHE_FLASH_ATTR OTAUpdater::UpdateProg( const bin_type type, bool restart ) {

    // Update program image
    char imageFileRequest[OTA_MAX_IMAGE_URL_LEN];
    strcpy(imageFileRequest, _assetRequestURL);
    strcat_P(imageFileRequest, PSTR("&asset="));
    strcat_P(imageFileRequest,  flag_DEVICE_CODE);
    strcat(imageFileRequest, _progSuffix);
    strcat_P(imageFileRequest, PSTR("&tag="));
    strcat(imageFileRequest, _latestTag);
    strcat_P(imageFileRequest, (type == GZ ? PSTR("&type=gz") : PSTR("")) );
    
    LOG(PSTR("(Updater) Updating Program"));
    LOGF_HIGH( PSTR("(Updater) Program image request: %s"), imageFileRequest );

    if( _settings->skipUpdates ) {

        LOG(PSTR("(Updater) Skipping update"));
        return UPDATE_SKIPPED;

    }
    else {
        
        ESPhttpUpdate.rebootOnUpdate(false);
        HTTPUpdateResult ret;
        ret = ESPhttpUpdate.update( *_client, imageFileRequest );

        switch(ret) {

            case HTTP_UPDATE_FAILED:
                LOGF_CRITICAL( PSTR("(Updater) Program update failed - Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str() );
                return PROG_UPDATE_FAILED;

            case HTTP_UPDATE_NO_UPDATES:
                LOG(PSTR("(Updater) No new program update"));
                return NO_UPDATES;
                
            default:
                LOG(PSTR("(Updater) Program updated successfully"));
        }

        if( restart ) {
            LOG_CRITICAL(PSTR("(Updater) Rebooting in 5 sec"));
            delay(5000);
            ESP.restart();
        }

        return UPDATE_OK;
    }

}



void OTAUpdater::handle() {

    if ( _doUpdateCheck && network.isInternetConnected() ) {

        _doUpdateCheck = false;

        if( !checkForUpdate() ) return;     // No new update
        
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

