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

Implements a location and time service using IPInfo.io and ezTime library.

JSON response from IPinfo.io is in this format:

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

#define TLO_IPINFO_JSON_RESPONSE_SIZE (JSON_OBJECT_SIZE(9) + 280)          // TODO - Do #defines for others like this.


#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ezTime.h>

#include "Device.h"
#include "Logger.h"
#include "NetworkManager.h"
#include "TimeLocation.h"


void ICACHE_FLASH_ATTR TimeLocation::begin( WiFiClient &client, TimeLocationSettings &settings ) {
    _client = &client;
    begin(settings);
}
void ICACHE_FLASH_ATTR TimeLocation::begin( TimeLocationSettings &settings ) {

    _settings = &settings;

    if( _settings->mode ) {

        LOG(PSTR("(TimeLoc) Starting time and location service"));

        _timezone = new Timezone;

    }

}


void TimeLocation::handle() {

    if( !_locationStatus && _timer < millis() && network.isInternetConnected() ) {
        _timer = millis() + TLO_IPINFO_RETRY;

        if( detectLocation() ) {
            LOGF( PSTR("(TimeLoc) Timezone set to: %s"), _location.timezone );
            _timezone->setLocation(_location.timezone);
        }
        else LOG( PSTR("(TimeLoc) Timezone not set, using UTC") );             
    }

    events();                       // ezTime handler

    _timeStatus = ( timeStatus() != timeNotSet );
}


bool ICACHE_FLASH_ATTR TimeLocation::detectLocation() {

    _locationStatus = false;

    if( _settings->ipinfoToken[0]=='\0' ) {
        LOG(PSTR("(TimeLoc) Mising IPInfo token"));
        return false;
    }

    HTTPClient http;

    char url[TLO_IPINFO_MAX_TOKEN_LEN+sizeof("http://ipinfo.io/json?token=")+1];        // TODO - Build flag?
    strcpy_P(url,"http://ipinfo.io/json?token=");
    strcat(url,_settings->ipinfoToken);

    http.useHTTP10(true);
    if( !http.begin(*_client,url) ) {
        LOG(PSTR("(TimeLoc) HTTP error getting location"));
        return false;
    }
    int httpCode = http.GET();                                                         

    StaticJsonDocument<TLO_IPINFO_JSON_RESPONSE_SIZE> json;
    DeserializationError jsonerror = deserializeJson(json, http.getStream());                            // TODO - implement error handling
    http.end();

    if( httpCode == HTTP_CODE_OK ) LOG_HIGH(PSTR("(TimeLoc) Location received"));
    else {
        LOG(PSTR("(TimeLoc) HTTP error getting location"));
        return false;
    }

    if (jsonerror) {
        LOGF_CRITICAL( PSTR("(TimeLoc) JSON Error: %s"), jsonerror.c_str() );
        return false;
    }

    _location.ip.fromString(json[F("ip")].as<char*>());
    strcpy(_location.city,json[F("city")].as<char*>());
    strcpy(_location.region,json[F("region")].as<char*>());
    strcpy(_location.country,json[F("country")].as<char*>());
    
    std::vector<char*> v;
    char* array = strtok(_location.country,json[F("loc")].as<char*>());
    while(array) {
        v.push_back(array);
        array = strtok(NULL,",");
    }
    _location.loc.lon = atof(v[0]);
    _location.loc.lat = atof(v[1]);

    strcpy(_location.postal,json[F("postal")].as<char*>());
    strcpy(_location.timezone,json[F("timezone")].as<char*>());

    _locationStatus = true;

    return true;            

}


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TIMELOCATION)
    TimeLocation timelocation;
#endif

