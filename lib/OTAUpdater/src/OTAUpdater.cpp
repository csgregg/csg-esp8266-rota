/**
 * @file        OTAUpdater.cpp
 * @author      Chris Gregg
 * 
 * @brief       Implements a remote update service for ESP using GitHub
 *              to store code and binaries built by Travis-CI.
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
#include <ArduinoJson.h>

// Project Libraries
#include "IOTDevice.h"
#include "Logger.h"
#include "OTAUpdater.h"
#include "NetworkManager.h"


/* JSON used to get versioning from GitHub is in this format:

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


////////////////////////////////////////////
//// OTA Updater Settings Class

// Public:

// Resest the OTA updater settings to defaults
void ICACHE_FLASH_ATTR OTAUpdaterSettings::SetDefaults() {
    enabled = OTA_DEFAULT_MODE;
    strcpy_P( service, flag_UPDATER_SERVICE );
    strcpy_P( repo, flag_UPDATER_REPO );
    strcpy_P( user, flag_UPDATER_USER );
    strcpy_P( token, flag_UPDATER_TOKEN );
    interval = flag_UPDATER_INTERVAL;
    skipUpdates = flag_UPDATER_SKIP;
}


////////////////////////////////////////////
//// OTA Updater Manager Class

// Public:

// Initializes the OTA updater service
void ICACHE_FLASH_ATTR OTAUpdater::Begin( WiFiClient& client, OTAUpdaterSettings& settings ) {
    _client = &client;
    Restart(settings);
}


// Reloads the settings for the OTA service
void ICACHE_FLASH_ATTR OTAUpdater::Restart( OTAUpdaterSettings& settings ) {

    _settings = &settings;
    _doUpdateCheck = false;

    if( _updateCheck.active() ) _updateCheck.detach();

    if( _settings->enabled ) {

        // Build request URL
        strcpy_P( _assetRequestURL, PSTR("http://") );
        strcat( _assetRequestURL, _settings->service );
        strcat_P( _assetRequestURL, PSTR("?repo=") );
        strcat( _assetRequestURL, _settings->repo );
        strcat_P( _assetRequestURL, PSTR("&user=") );
        strcat( _assetRequestURL, _settings->user );

        // Add token if it is saved
        if( _settings->token[0] != '\0' ) {
            strcat_P( _assetRequestURL, PSTR("&token=") );
            strcat( _assetRequestURL, _settings->token );
        }

        LOG( PSTR("(Updater) Starting auto updater") );

        _updateCheck.attach( _settings->interval, TriggerUpdateCheck );     // Start timer
    }

}


// Handles any repeating tasks for the OTA update service
void OTAUpdater::Handle() {

    if ( _doUpdateCheck && ( network.GetNetworkStatus() == NetworkManager::NetworkStatus::NORMAL ) ) {

        _doUpdateCheck = false;

        if( !IsUpdateAvailable() ) return;     // No new update
        
        UpdateProg( GZ, true );
        
    }
}


// Protected:



// Perform the update check
bool ICACHE_FLASH_ATTR OTAUpdater::IsUpdateAvailable() {

    LOG( PSTR("(Updater) Checking latest build") );
    LOGF_HIGH( PSTR("(Updater) URL: %s"), _assetRequestURL );

    // Fetch the details from the update services
    HTTPClient http;

    http.setReuse(false);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setUserAgent(FPSTR(flag_DEVICE_CODE));
    http.useHTTP10(true);
    http.begin( *_client, _assetRequestURL );
    int httpcode = http.GET();

    // Did we get a result?
    if( httpcode != HTTP_CODE_OK ) {
        if( httpcode < 0 ) LOGF_CRITICAL( PSTR("(Updater) Error getting latest release: ERROR %s"), http.errorToString(httpcode).c_str() );
        else LOGF_CRITICAL( PSTR("(Updater) Error getting latest release: ERROR %i"), httpcode );
        return false;
    }

    // Expecting JSON back with latest release details
    DynamicJsonDocument responseJSON(OTA_CHECK_JSON_RESPONSE_SIZE);
    DeserializationError jsonerror = deserializeJson( responseJSON, http.getStream() );
    http.end();

    // Did we get clean JSON?
    if (jsonerror) {
        LOGF_CRITICAL( PSTR("(Updater) JSON Error: %s"), jsonerror.c_str() );
        return false;
    }

    const char* repoName = responseJSON[F("repo")];         // The repo name returned

    LOGF_HIGH( PSTR("(Updater) Returned Repo: %s"), repoName );

    if( strcmp( repoName, _settings->repo ) != 0 ) {
        LOG_CRITICAL(PSTR("(Updater) JSON Error getting latest release"));
        return false;
    }

    // Get the latest release
    JsonObject latestRelease = responseJSON[F("releases")][0];
    char currentTag[OTA_MAX_TAG_LEN];
    strcpy_P( currentTag,flag_BUILD_TAG );
    const char* latestTag = latestRelease[F("tag")];
    const char* releaseDate = latestRelease[F("date")];

    // Did we get a release?
    if( latestTag[0] == '\0' ) {
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

    // Store detailed for latest release
    strcpy( _latestTag, latestTag );
    strcpy( _latestReleaseDate, releaseDate );

    return true;            // Return success

}




// Update the program firmware from image stored at GitHub
OTAUpdater::UpdateResult ICACHE_FLASH_ATTR OTAUpdater::UpdateProg( const BinType type, bool doRestart ) {

    // Build program image asset URL
    char progFileRequest[OTA_MAX_IMG_URL_LEN];
    strcpy( progFileRequest, _assetRequestURL );
    strcat_P( progFileRequest, PSTR("&asset=") );
    strcat_P( progFileRequest,  flag_DEVICE_CODE);
    strcat_P( progFileRequest, PSTR("-Pv") );
    strcat_P( progFileRequest, PSTR("&tag=") );
    strcat( progFileRequest, _latestTag);
    strcat_P( progFileRequest, ( type == GZ ? PSTR("&type=gz") : PSTR("") ) );
    
    LOG( PSTR("(Updater) Updating Program") );
    LOGF_HIGH( PSTR("(Updater) Program image request: %s"), progFileRequest );

    if( _settings->skipUpdates ) {
        LOG(PSTR("(Updater) Skipping update"));
        return UPDATE_SKIPPED;
    }
    else {
        
        ESPhttpUpdate.rebootOnUpdate( false );
        HTTPUpdateResult ret;
        ret = ESPhttpUpdate.update( *_client, progFileRequest );

        switch( ret ) {

            case HTTP_UPDATE_FAILED:
                LOGF_CRITICAL( PSTR("(Updater) Program update failed - Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str() );
                return PROG_UPDATE_FAILED;

            case HTTP_UPDATE_NO_UPDATES:
                LOG( PSTR("(Updater) No new program update") );
                return NO_UPDATES;
                
            default:
                LOG( PSTR("(Updater) Program updated successfully") );
        }

        if( doRestart ) {
            LOG_CRITICAL( PSTR("(Updater) Rebooting in 5 sec") );
            delay(5000);
            ESP.restart();
        }

        return UPDATE_OK;
    }

}


bool OTAUpdater::_doUpdateCheck = false;        // Intialize static member
OTAUpdater updater;                             // Create the global instance
