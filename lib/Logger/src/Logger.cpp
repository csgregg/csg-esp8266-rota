/* Logger Library

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

Packages debug and logging to the Loggly.com service for simple logging or
debug of code. Macros are defined for to simplifiy common usage. 

THe Loggly service requires a JSON file :

JSON Loggining Format

{
  "localtime": 1234567890,
  "message": "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456",
  "Device": {
    "Hardware": {
      "Board": "12345678901234567890",
      "MAC": "mm:mm:mm:ss:ss:ss"
    },
    "Env": {
      "Name": "123456789012345678901234567890",
      "Code": "12345678901234567890",
      "Build": "xx.xx.xx",
      "Heap": "1234567890"
     },
     "Network": {
       "IPAddress": "123456789012345",
       "SSID": "12345678901234567890123456789012"
     }
  }
}

Use https://arduinojson.org/v6/assistant/ to determine size of file.

*/


#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "Logger.h"
#include "Device.h"


bool LogClient::_doTick = false;


void ICACHE_FLASH_ATTR LogSettings::setDefaults() {
    serialBaud = flag_MONITOR_SPEED;
    strcpy_P(serviceURL,flag_LOGGER_SERVICE);
    strcpy_P(serviceKey,flag_LOGGER_SERVICE_KEY);
    serialMode = flag_LOGGER_AS_SERIAL;
    serviceMode = flag_LOGGER_AS_SERVICE;
    strcpy_P(globalTags,flag_LOGGER_GLOBAL_TAGS);
    level = logLevel(flag_LOGGER_LEVEL);
    tickMode = flag_LOGGER_TICKER;
    tickInterval = flag_LOGGER_TICK_INTERVAL;
}



// Public:

// Sets up logger
void ICACHE_FLASH_ATTR LogClient::begin( LogSettings &settings ) {

     _settings = &settings;

#ifndef NO_LOGGING

    _doTick = false;
    if( _tickCheck.active() ) _tickCheck.detach();

    if( _settings->serviceMode || _settings->tickMode ) {

        strcpy_P(_FullServiceURL, PSTR("http://"));
        strcat(_FullServiceURL, _settings->serviceURL);
        strcat_P(_FullServiceURL, PSTR("/"));
        strcat(_FullServiceURL, _settings->serviceKey);
        strcat_P(_FullServiceURL, PSTR("/tag/"));
        strcat(_FullServiceURL, _settings->globalTags);
        strcat_P(_FullServiceURL, PSTR("/"));
    }

    if( _settings->serialMode ) {
        delay(1000);
        Serial.begin(_settings->serialBaud);             
        Serial.println(PSTR("LOG: (Logger) Starting Logging"));
    }

    LOGF_HIGH( PSTR("(Logger) Logging set at level: %i"), _settings->level );

    if( _settings->serviceMode ) LOG_HIGH(PSTR("(Logger) Logging Service: ON"));

    if( _settings->tickMode ) {
        LOG_HIGH(PSTR("(Logger) Tick Service: ON"));
        _tickCheck.attach( _settings->tickInterval, TriggerTick );
    }

#endif

}
void ICACHE_FLASH_ATTR LogClient::begin( WiFiClient &client, LogSettings &settings ) {

    _client = &client;
    begin(settings);
   
}



// Main log function - char[]
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, const char * message ) {

#ifndef NO_LOGGING

    if( !_settings ) begin( _preSettings );

    if( uint(type) >= uint(_settings->level) ) return;

    if( _settings->serialMode ) LogToSerial(type, tag, message);
    if( _settings->serviceMode ) LogToService(type, tag, message);

#endif

}


// Overload - with context
void ICACHE_FLASH_ATTR LogClient::println(const logType type, const logTag tag, const char * message, const char * file, const char * func_P, const int line ) {

#ifndef NO_LOGGING

    if( !_settings ) begin( _preSettings );

    if( _settings->level == LOGGING_LEVEL_VERBOSE ) {

        char str[LOG_MAX_MESSAGE_LEN];
        snprintf_P(str, LOG_MAX_MESSAGE_LEN, PSTR("(Context: %s %s %i) %s"), file, func_P, line, message);

        println(type, tag, str);
    }
    else println(type, tag, message);

#endif

}



// Overload println() - char
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, int i ) {

#ifndef NO_LOGGING

    char buff[12];
    sprintf_P(buff,PSTR("%i"),i);
    println(type, tag, buff);

#endif

}


// Overload println() - char with context
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, int i, const char * file, const char * func_P, const int line ) {

#ifndef NO_LOGGING

    char buff[12];
    sprintf_P(buff,PSTR("%i"),i);
    println(type, tag, buff, file, func_P, line);

#endif

}



// Overload println() - char
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, char c ) {

#ifndef NO_LOGGING

    char c_str[2];
    c_str[0] = c;
    c_str[1] = 0;

    println(type, tag, c_str);

#endif

}


// Overload println() - char with context
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, char c, const char * file, const char * func_P, const int line ) {

#ifndef NO_LOGGING

    char c_str[2];
    c_str[0] = c;
    c_str[1] = 0;
  
    println(type, tag, c_str, file, func_P, line);

#endif

}


// Overload println() - string
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, const String &s ) {

#ifndef NO_LOGGING

    println( type, tag, s.c_str() );

#endif

}


// Overload println() - string with context
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, const String &s, const char * file, const char * func_P, const int line ) {

#ifndef NO_LOGGING

    println( type, tag, s.c_str(), file, func_P, line  );

#endif

}



// Formatted log function
void ICACHE_FLASH_ATTR LogClient::printf( const logType type, const logTag tag, const char * format, ... ) {

#ifndef NO_LOGGING

    va_list arg;
    va_start(arg, format);
    char temp[LOG_MAX_MESSAGE_LEN];
    char* buffer = temp;

    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            println(CRITICAL_LOG, STATUS_TAG, PSTR("(Logger) LogClient: Buffer error"));
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }

    if (buffer != temp) {
        delete[] buffer;
    }

    println(type, tag, temp);

#endif

}


// Formatted log function with context
void ICACHE_FLASH_ATTR LogClient::printf( const logType type, const logTag tag, const char * file, const char * func_P, const int line, const char * format, ... ) {

#ifndef NO_LOGGING

    va_list arg;
    va_start(arg, format);
    char temp[LOG_MAX_MESSAGE_LEN];
    char* buffer = temp;

    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            println(CRITICAL_LOG, STATUS_TAG, PSTR("(Logger) LogClient: Buffer error"));
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }

    if (buffer != temp) {
        delete[] buffer;
    }

    println(type, tag, temp, file, func_P, line);

#endif

}



// Prints build flag - overload char[]
void ICACHE_FLASH_ATTR LogClient::printFlag(const logType type, const logTag tag, const char* name, const char* flag) {

#ifndef NO_LOGGING

    char buffer[LOG_MAX_MESSAGE_LEN];
    snprintf_P(buffer, LOG_MAX_MESSAGE_LEN, PSTR("(Build) %s: %s"), name, FPSTR(flag));

    println(type, tag, buffer);

#endif
}

// Prints build flag - overload bool
void ICACHE_FLASH_ATTR LogClient::printFlag(const logType type, const logTag tag, const char* name, const bool flag) {

#ifndef NO_LOGGING

    char buffer[LOG_MAX_MESSAGE_LEN];
    snprintf_P(buffer, LOG_MAX_MESSAGE_LEN, PSTR("(Build) %s: %i"), name, flag);

    println(type, tag, buffer);

#endif
}

// Prints build flag - overload uint
void ICACHE_FLASH_ATTR LogClient::printFlag(const logType type, const logTag tag, const char* name, const uint flag) {

#ifndef NO_LOGGING

    char buffer[LOG_MAX_MESSAGE_LEN];
    snprintf_P(buffer, LOG_MAX_MESSAGE_LEN, PSTR("(Build) %s: %i"), name, flag);

    println(type, tag, buffer);

#endif
}



// Protected:

// Create and log prefix - needs to be followed by message using Serial.println()
void LogClient::LogPrefix( const logType type, const logTag tag ){

#ifndef NO_LOGGING

    if( _settings->level == LOGGING_LEVEL_VERBOSE) Serial.printf_P(PSTR("%s: %s - Millis: %li, Heap: %i - "), c_log_tag_descript[tag], c_log_type_descript[type], millis(), system_get_free_heap_size());
    else Serial.printf_P(PSTR("%s: %s - "), c_log_tag_descript[tag], c_log_type_descript[type]);

#endif

}


// Log message to serial
void ICACHE_FLASH_ATTR LogClient::LogToSerial( logType type, logTag tag, const char * message ){

#ifndef NO_LOGGING

    char shortened[LOG_MAX_MESSAGE_LEN+1];

    strncpy( shortened, message, LOG_MAX_MESSAGE_LEN );        // Truncate if too long

    LogPrefix(type, tag);
    Serial.println(shortened);

#endif

}


// Log message to Loggly Service
void ICACHE_FLASH_ATTR LogClient::LogToService( const logType type, const logTag tag, const char * message ){

#ifndef NO_LOGGING

    if( WiFi.status() != WL_CONNECTED ) return;

    char thistype[LOG_MAX_TYPE_DESC_LEN];
    strcpy_P(thistype, c_log_type_descript[type]);

    char loggingURL[strlen(_FullServiceURL)+LOG_MAX_TAG_DESC_LEN+2];
    strcpy(loggingURL,_FullServiceURL);
    strcat_P(loggingURL,c_log_tag_descript[tag]);
    strcat_P(loggingURL,PSTR("/"));
   
    if( _settings->serialMode && _settings->level == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(DETAIL_LOG, STATUS_TAG);
        Serial.print(PSTR("(Logger) Logging to: "));
        Serial.println(loggingURL);
    }

    // Build JSON
    // Use https://arduinojson.org/v6/assistant/
    
    const size_t capacity = 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 509;
    DynamicJsonDocument jsonLog(capacity);

    jsonLog[F("localtime")] = millis();

    char shortened[LOG_MAX_MESSAGE_LEN];

    strncpy( shortened, message, LOG_MAX_MESSAGE_LEN );        // Truncate if too long

    jsonLog[F("message")] = shortened;

    JsonObject Device = jsonLog.createNestedObject(F("Device"));

        JsonObject Device_Hardware = Device.createNestedObject(F("Hardware"));
        Device_Hardware[F("Board")] = FPSTR(flag_BOARD);
        String tempMAC = WiFi.macAddress(); Device_Hardware[F("MAC")] =  tempMAC.c_str();

        JsonObject Device_Env = Device.createNestedObject(F("Env"));
        Device_Env[F("Name")] = FPSTR(flag_DEVICE_NAME);
        Device_Env[F("Code")] = FPSTR(flag_DEVICE_CODE);
        Device_Env[F("Build")] = FPSTR(flag_BUILD_ENV);
        Device_Env[F("Tag")] = FPSTR(flag_BUILD_TAG);
        Device_Env[F("Heap")] = system_get_free_heap_size();

        JsonObject Device_Network = Device.createNestedObject(F("Network"));

        String tempIP = WiFi.localIP().toString();
        Device_Network[F("IPAddress")] = tempIP.c_str();

        String tempSSID = WiFi.SSID();
        Device_Network[F("SSID")] = tempSSID.c_str();

    char jsonMessage[712+1];        // TODO - check this    

    serializeJson(jsonLog, jsonMessage);

    if( _settings->serialMode && _settings->level == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(DETAIL_LOG, STATUS_TAG);
        Serial.print(PSTR("(Logger) Log (JSON): "));
        Serial.println(jsonMessage); 
    }


    // Start the connection
    
    HTTPClient http;

    if( !http.begin(*_client, loggingURL) ) {
        if( _settings->serialMode && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix(CRITICAL_LOG, STATUS_TAG);
            Serial.print(PSTR("(Logger) Logging to servce: HTTP Client Error "));
        }
    }

    http.setUserAgent(FPSTR(flag_DEVICE_CODE));
    http.addHeader(PSTR("Content-Type"), PSTR("text/plain"));

    int httpCode = http.POST(jsonMessage);
    http.end();

    if( httpCode == HTTP_CODE_OK ) {
        if( _settings->serialMode && _settings->level == LOGGING_LEVEL_VERBOSE ) {
            LogPrefix(DETAIL_LOG, STATUS_TAG);
            Serial.println(PSTR("(Logger) Logging to servce: SUCCESS "));
        }
    }
    else {
        if( _settings->serialMode && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix(CRITICAL_LOG, STATUS_TAG);
            Serial.print(PSTR("(Logger) Logging to servce: ERROR "));
            if( httpCode < 0 ) Serial.println( http.errorToString(httpCode).c_str() );
            else Serial.println( httpCode );
        }
    }

#endif

}


// Send tick to Loggly Service
bool LogClient::handleTick( ){

#ifndef NO_LOGGING

    if( WiFi.status() != WL_CONNECTED ) return false;

    if( _settings->serialMode && _settings->level >= LOGGING_LEVEL_NORMAL ) {
        LogPrefix(NORMAL_LOG, STATUS_TAG);
        Serial.println(PSTR("(Logger) Logging a tick")); 
    }

    char thistype[LOG_MAX_TYPE_DESC_LEN];
    strcpy_P(thistype, c_log_type_descript[NORMAL_LOG]);

    char loggingURL[strlen(_FullServiceURL)+LOG_MAX_TAG_DESC_LEN+2];
    strcpy(loggingURL,_FullServiceURL);
    strcat_P(loggingURL,c_log_tag_descript[STATUS_TAG]);
    strcat_P(loggingURL,PSTR("/"));

    // Build JSON
    // Use https://arduinojson.org/v6/assistant/
    
    const size_t capacity = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 244;
    DynamicJsonDocument jsonLog(capacity);

    jsonLog[F("localtime")] = millis();

    JsonObject Device = jsonLog.createNestedObject(F("Device"));

        JsonObject Device_Hardware = Device.createNestedObject(F("Hardware"));
        Device_Hardware[F("Board")] = FPSTR(flag_BOARD);
        Device_Hardware[F("MAC")] = WiFi.macAddress();

        JsonObject Device_Env = Device.createNestedObject(F("Env"));
        Device_Env[F("Name")] = FPSTR(flag_DEVICE_NAME);
        Device_Env[F("Code")] = FPSTR(flag_DEVICE_CODE);
        Device_Env[F("Build")] = FPSTR(flag_BUILD_ENV);
        Device_Env[F("Tag")] = FPSTR(flag_BUILD_TAG);
        Device_Env[F("Heap")] = system_get_free_heap_size();

        JsonObject Device_Network = Device.createNestedObject(F("Network"));

        String tempIP = WiFi.localIP().toString();
        Device_Network[F("IPAddress")] = tempIP.c_str();

        String tempSSID = WiFi.SSID();
        Device_Network[F("SSID")] = tempSSID.c_str();

    char jsonMessage[438+1];    // TODO - #define for size

    serializeJson(jsonLog, jsonMessage);

    // Start the connection
    
    HTTPClient http;

    if( !http.begin(*_client, loggingURL) ) {
        if( _settings->serialMode && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix(CRITICAL_LOG, STATUS_TAG);
            Serial.print(PSTR("(Logger) Logging to servce: HTTP Client Error "));
        }
    }
    
    http.setUserAgent(FPSTR(flag_DEVICE_CODE));
    http.addHeader(PSTR("Content-Type"), PSTR("text/plain"));

    int httpCode = http.POST(jsonMessage);
    http.end();

    return (httpCode == HTTP_CODE_OK );

#else
    return false;
#endif

}


void LogClient::TriggerTick() {
    _doTick = true;
}


void LogClient::handle() {

#ifndef NO_LOGGING

    if ( _settings->tickMode && _doTick && WiFi.status() == WL_CONNECTED ) {

        _doTick = false;

        handleTick();
    }

#endif

}


// Create the global logger instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif