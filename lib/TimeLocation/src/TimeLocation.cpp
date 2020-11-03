/**
 * @file        TimeLocation.cpp
 * @author      Chris Gregg
 * 
 * @brief       Implements a location and time service using IPInfo.io and ezTime library.
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
#include <ESP8266HTTPClient.h>
#include <ezTime.h>

// Project Libraries
#include "IOTDevice.h"
#include "Logger.h"
#include "NetworkManager.h"
#include "TimeLocation.h"


/* JSON response from IPinfo.io is in this format:

{
  "ip": "123.123.123.123",
  "hostname": "1234567890123456789012345678901234567890123456789012345678901234",
  "city": "1234567890123456",
  "region": "1234567890123456",
  "country": "12",
  "loc": "-123.1234,-123.1234",
  "org": "12345678901234567890123456789012",
  "postal": "12345678",
  "timezone": "12345678901234/12345678901234"
}

https://arduinojson.org/v6/assistant/

*/

#define TLO_IPINFO_JSON_RESPONSE_SIZE (JSON_OBJECT_SIZE(9) + 280)


////////////////////////////////////////////
//// TimeLocatinSettings Class

// Public:

// Resets location to default
void ICACHE_FLASH_ATTR Location::SetDefaults() {
    publicIP = { 0, 0, 0, 0 };
    strcpy_P( city, PSTR("") );
    strcpy_P( region, PSTR("") );
    strcpy_P( country, PSTR("") );
    coords = { 0, 0 };
    strcpy_P( postal, PSTR("") );
    strcpy_P( timezone, PSTR("") );
}


////////////////////////////////////////////
//// TimeLocationSettings Class

// Public:

// Resets time and location settings to default
void ICACHE_FLASH_ATTR TimeLocationSettings::SetDefaults() {
    enabled = true;
    strcpy_P( ipInfoToken, flag_TLO_IPINFO_TOKEN );
    strcpy_P( posix, PSTR("UTC") );
    location.SetDefaults();
}


////////////////////////////////////////////
//// TimeLocationSettings Class

// Public:

// Initializes the Time and Location serices
void ICACHE_FLASH_ATTR TimeLocationManager::Begin( WiFiClient& client, TimeLocationSettings& settings ) {
    _client = &client;
    if( _timezone ) delete _timezone;
    else _timezone = new Timezone;
    Restart( settings );
}


// Restart the time and location services
void ICACHE_FLASH_ATTR TimeLocationManager::Restart( TimeLocationSettings& settings ) {

    LOG(PSTR("(TimeLoc) Starting time and location service"));

    _settings = &settings;

    if( _settings->location.region[0] != '\0' ) {
        LOGF_HIGH( PSTR("(TimeLoc) Setting timezone to: %s"), _settings->location.timezone );
        _isLocationSet = _timezone->setPosix( _settings->posix );
    }
    else {
        _settings->location.SetDefaults();
        _isLocationSet = false;
    }

}


// Handles any repeasting time and location tasks
void TimeLocationManager::Handle() {

    if( _settings->enabled ) {
        if( network.GetNetworkStatus() == NetworkManager::NetworkStatus::NORMAL ) {
            if( !_wasPreviousConnected ) {
                _wasPreviousConnected = true;
                ezt::updateNTP();
            }
        }
        else _wasPreviousConnected = false;

        ezt::events();              // ezTime handler
    }                   

    _isTimeSet = ( ezt::timeStatus() != timeNotSet );

}


// Detect location using IPInfo.io
bool ICACHE_FLASH_ATTR TimeLocationManager::DetectLocation() {

    LOG_HIGH( PSTR("(TimeLoc) Detecting location") );

    _isLocationSet = false;

    if( _settings->ipInfoToken[0] == '\0' ) {
        LOG( PSTR("(TimeLoc) Missing IPInfo token") );
        return false;
    }

    // Build URL for IPInfo.io request
    char url[sizeof("http://")+sizeof(flag_TLO_IPINFO_SERVICE)+TLO_IPINFO_MAX_TOKEN_LEN];
    strcpy_P(url,PSTR("http://"));
    strcat_P(url,flag_TLO_IPINFO_SERVICE);
    strcat(url,_settings->ipInfoToken);

    // Start connection
    HTTPClient http;
    http.setUserAgent( FPSTR(flag_DEVICE_CODE) );
    http.useHTTP10( true );
    if( !http.begin( *_client, url ) ) {
        LOG( PSTR("(TimeLoc) HTTP error getting location") );
        return false;
    }
    int httpCode = http.GET();                                                         

    // Deserialize response
    StaticJsonDocument<TLO_IPINFO_JSON_RESPONSE_SIZE> json;
    DeserializationError jsonerror = deserializeJson( json, http.getStream() );
    http.end();

    // Did we get a response
    if( httpCode == HTTP_CODE_OK ) LOG_HIGH( PSTR("(TimeLoc) Location received") );
    else {
        LOG( PSTR("(TimeLoc) HTTP error getting location") );
        return false;
    }

    // Is the JSON valid
    if( jsonerror ) {
        LOGF_CRITICAL( PSTR("(TimeLoc) JSON Error: %s"), jsonerror.c_str() );
        return false;
    }

    // Get the details from the response
    _settings->location.publicIP.fromString( json[F("ip")].as<char*>() );
    strcpy( _settings->location.city, json[F("city")].as<char*>() );
    strcpy( _settings->location.region, json[F("region")].as<char*>() );
    strcpy( _settings->location.country, json[F("country")].as<char*>() );

    char buff[TLO_IPINFO_MAX_LOC_LEN];
    strcpy( buff, json[F("loc")].as<char*>() );
    int len = ( strchr( buff, ',' ) - buff ) * sizeof(char);
    char newbuff[TLO_IPINFO_MAX_LOC_LEN];
    _settings->location.coords.lon =  atof( strncpy( newbuff, buff, len ) );
    _settings->location.coords.lat =  atof( strcpy( newbuff, buff+len+1 ) );

    strcpy( _settings->location.postal, json[F("postal")].as<char*>() );
    strcpy( _settings->location.timezone, json[F("timezone")].as<char*>() );

    _isLocationSet = _timezone->setLocation( _settings->location.timezone );
    strcpy( _settings->posix,_timezone->getPosix().c_str() );

    if( _isLocationSet ) LOGF_HIGH( PSTR("(TimeLoc) Timezone set to: %s"), _settings->location.timezone );
    else LOG( PSTR("(TimeLoc) Timezone not set") );

    return _isLocationSet;            
}


// Update the time using NTP
void ICACHE_FLASH_ATTR TimeLocationManager::UpdateTime() {
    ezt::updateNTP();
}


// Copy the time date into a char array
void ICACHE_FLASH_ATTR TimeLocationManager::StrcpyTimeDate( char* datetimestring ) {
    strncpy( datetimestring, _timezone->dateTime().c_str(), TLO_MAX_LONG_DATETIME_LEN );
}

void ICACHE_FLASH_ATTR TimeLocationManager:: StrncpyTimeDate( char* datetimestring, size_t len ) {
    strncpy( datetimestring, _timezone->dateTime().c_str(), len < TLO_MAX_LONG_DATETIME_LEN ? len : TLO_MAX_LONG_DATETIME_LEN );
}


TimeLocationManager timelocation;           // Create the global instance
