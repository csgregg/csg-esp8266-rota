/**
 * @file        Logger.cpp
 * @author      Chris Gregg
 * @brief       Packages debug and logging to the Loggly.com service for simple logging or
 *              debug of code. Macros are defined for to simplifiy common usage.
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
#include <ArduinoJson.h>

// Project Libraries
#include "Logger.h"
#include "IOTDevice.h"


/* The Loggly service requires a JSON file :

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

#define LOG_TICK_JSON_SIZE (3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 244)
#define LOG_SERVICE_JSON_SIZE (2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 509)


////////////////////////////////////////////
//// Logger Settings Class

// Public:

// Resest the logger settings to defaults
void ICACHE_FLASH_ATTR LoggerSettings::SetDefaults() {
    serialBaud = flag_MONITOR_SPEED;
    strcpy_P( serviceURL, flag_LOGGER_SERVICE );
    strcpy_P( serviceKey, flag_LOGGER_SERVICE_KEY );
    serialModeOn = flag_LOGGER_AS_SERIAL;
    serviceModeOn = flag_LOGGER_AS_SERVICE;
    strcpy_P( globalTags, flag_LOGGER_GLOBAL_TAGS );
    level = LogLevel(flag_LOGGER_LEVEL);
    tickModeOn = flag_LOGGER_TICKER;
    tickInterval = flag_LOGGER_TICK_INTERVAL;
}


////////////////////////////////////////////
//// Logger Settings Class

// Public:

// Sets up logging service
void ICACHE_FLASH_ATTR LogClient::Begin( WiFiClient& client, LoggerSettings& settings )
{
    _client = &client;
    Restart( settings );
}


// Restarts the logging service
void ICACHE_FLASH_ATTR LogClient::Restart( LoggerSettings& settings )
{
     _settings = &settings;

#ifndef NO_LOGGING

    // Clear any ticker timer
    _doTick = false;
    if( _tickCheck.active() ) _tickCheck.detach();

    // Build the Loggly URL
    if( _settings->serviceModeOn || _settings->tickModeOn ) {
        strcpy_P( _fullServiceURL, PSTR("http://") );
        strcat( _fullServiceURL, _settings->serviceURL );
        strcat_P( _fullServiceURL, PSTR("/"));
        strcat( _fullServiceURL, _settings->serviceKey );
        strcat_P( _fullServiceURL, PSTR("/tag/"));
        strcat( _fullServiceURL, _settings->globalTags );
        strcat_P( _fullServiceURL, PSTR("/") );
    }

    if( _settings->serialModeOn ) {
        delay( 1000 );
        Serial.begin( _settings->serialBaud );             
        Serial.println( PSTR("LOG: (Logger) Starting Logging") );
    }

    LOGF_HIGH( PSTR("(Logger) Logging set at level: %i"), _settings->level );

    if( _settings->serviceModeOn ) LOG_HIGH( PSTR("(Logger) Logging Service: ON") );

    // Start the tick timer
    if( _settings->tickModeOn ) {
        LOG_HIGH(PSTR("(Logger) Tick Service: ON"));
        _tickCheck.attach( _settings->tickInterval, TriggerTick );
    }

#endif

}


// Send log messages - base println function char array
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, const char* message ) {

#ifndef NO_LOGGING

    if( uint(type) >= uint(_settings->level) ) return;

    if( _settings->serialModeOn ) LogToSerial( type, tag, message );
    if( _settings->serviceModeOn ) LogToService( type, tag, message );

#endif

}


// Send log messages - println function overload char array with context
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, const char* message, const char* file, const char* func_P, const int line ) {

#ifndef NO_LOGGING

    if( _settings->level == LOGGING_LEVEL_VERBOSE ) {
        char str[LOG_MAX_MESSAGE_LEN];
        snprintf_P( str, LOG_MAX_MESSAGE_LEN, PSTR("(Context: %s %s %i) %s"), file, func_P, line, message );
        println( type, tag, str );
    }
    else println( type, tag, message );

#endif

}


// Send log messages - println function overload int
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, int i ) {

#ifndef NO_LOGGING

    char buff[12];
    sprintf_P( buff, PSTR("%i"), i );
    println( type, tag, buff );

#endif

}


// Send log messages - println function overload int with context
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, int i, const char* file, const char* func_P, const int line ) {

#ifndef NO_LOGGING

    char buff[12];
    sprintf_P( buff, PSTR("%i"), i );
    println( type, tag, buff, file, func_P, line );

#endif

}


// Send log messages - println function overload single char
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, char c ) {

#ifndef NO_LOGGING

    char c_str[2];
    c_str[0] = c;
    c_str[1] = 0;

    println( type, tag, c_str );

#endif

}


// Send log messages - println function overload single char with context
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, char c, const char* file, const char* func_P, const int line ) {

#ifndef NO_LOGGING

    char c_str[2];
    c_str[0] = c;
    c_str[1] = 0;
  
    println( type, tag, c_str, file, func_P, line );

#endif

}


// Send log messages - println function overload String
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, const String& s ) {

#ifndef NO_LOGGING

    println( type, tag, s.c_str() );

#endif

}


// Send log messages - println function overload String with context
void ICACHE_FLASH_ATTR LogClient::println( const LogType type, const LogTag tag, const String& s, const char* file, const char* func_P, const int line ) {

#ifndef NO_LOGGING

    println( type, tag, s.c_str(), file, func_P, line );

#endif

}


// Send log messages - base printf function
void ICACHE_FLASH_ATTR LogClient::printf( const LogType type, const LogTag tag, const char* format, ... ) {

#ifndef NO_LOGGING

    va_list arg;
    va_start(arg, format);
    char temp[LOG_MAX_MESSAGE_LEN];
    char* buffer = temp;

    size_t len = vsnprintf( temp, sizeof(temp), format, arg );
    va_end( arg );
    if( len > sizeof(temp) - 1 ) {
        buffer = new char[len + 1];
        if( !buffer ) {
            println( CRITICAL_LOG, STATUS_TAG, PSTR("(Logger) LogClient: Buffer error") );
            return;
        }
        va_start( arg, format );
        vsnprintf( buffer, len + 1, format, arg );
        va_end( arg );
    }

    if( buffer != temp ) {
        delete[] buffer;
    }

    println( type, tag, temp );

#endif

}


// Send log messages - printf function overload with context
void ICACHE_FLASH_ATTR LogClient::printf( const LogType type, const LogTag tag, const char* file, const char* func_P, const int line, const char* format, ... ) {

#ifndef NO_LOGGING

    va_list arg;
    va_start( arg, format );
    char temp[LOG_MAX_MESSAGE_LEN];
    char* buffer = temp;

    size_t len = vsnprintf( temp, sizeof(temp), format, arg );
    va_end( arg );
    if( len > sizeof(temp) - 1 ) {
        buffer = new char[len + 1];
        if( !buffer ) {
            println( CRITICAL_LOG, STATUS_TAG, PSTR("(Logger) LogClient: Buffer error") );
            return;
        }
        va_start( arg, format );
        vsnprintf( buffer, len + 1, format, arg );
        va_end( arg );
    }

    if( buffer != temp ) {
        delete[] buffer;
    }

    println( type, tag, temp, file, func_P, line );

#endif

}


// Send log messages - base printFlag function char array flag
void ICACHE_FLASH_ATTR LogClient::printFlag( const LogType type, const LogTag tag, const char* name, const char* flag ) {

#ifndef NO_LOGGING

    char buffer[LOG_MAX_MESSAGE_LEN];
    snprintf_P( buffer, LOG_MAX_MESSAGE_LEN, PSTR("(Build) %s: %s"), name, FPSTR(flag) );
    println( type, tag, buffer );

#endif
}


// Send log messages - printFlag function overload bool flag
void ICACHE_FLASH_ATTR LogClient::printFlag( const LogType type, const LogTag tag, const char* name, const bool flag ) {

#ifndef NO_LOGGING

    char buffer[LOG_MAX_MESSAGE_LEN];
    snprintf_P( buffer, LOG_MAX_MESSAGE_LEN, PSTR("(Build) %s: %i"), name, flag );
    println( type, tag, buffer );

#endif
}


// Send log messages - printFlag function overload uint flag
void ICACHE_FLASH_ATTR LogClient::printFlag( const LogType type, const LogTag tag, const char* name, const uint flag ) {

#ifndef NO_LOGGING

    char buffer[LOG_MAX_MESSAGE_LEN];
    snprintf_P( buffer, LOG_MAX_MESSAGE_LEN, PSTR("(Build) %s: %i"), name, flag );
    println( type, tag, buffer );

#endif

}


// Handles any repeating device actions
void LogClient::Handle() {

#ifndef NO_LOGGING

    if ( _settings->tickModeOn && _doTick && WiFi.status() == WL_CONNECTED ) {
        _doTick = false;
        HandleTick();
    }

#endif

}


// Protected:

bool LogClient::_doTick = false;        // Initialize the static member

// Create and log prefix - needs to be followed by message using Serial.println()
void LogClient::LogPrefix( const LogType type, const LogTag tag ){

#ifndef NO_LOGGING

    if( _settings->level == LOGGING_LEVEL_VERBOSE ) Serial.printf_P( PSTR("%s: %s - Millis: %li, Heap: %i - "), _logTagDescriptions[tag], _logTypeDescriptions[type], millis(), system_get_free_heap_size() );
    else Serial.printf_P( PSTR("%s: %s - "), _logTagDescriptions[tag], _logTypeDescriptions[type] );

#endif

}


// Log message to serial
void ICACHE_FLASH_ATTR LogClient::LogToSerial( LogType type, LogTag tag, const char* message ){

#ifndef NO_LOGGING

    char shortened[LOG_MAX_MESSAGE_LEN+1];
    strncpy( shortened, message, LOG_MAX_MESSAGE_LEN );        // Truncate if too long
    LogPrefix( type, tag );
    Serial.println( shortened );

#endif

}


// Log message to Loggly Service
void ICACHE_FLASH_ATTR LogClient::LogToService( const LogType type, const LogTag tag, const char* message ){

#ifndef NO_LOGGING

    if( WiFi.status() != WL_CONNECTED ) return;             // TODO - used isconnected instead

    char thistype[LOG_MAX_TYPE_DESC_LEN];
    strcpy_P( thistype, _logTypeDescriptions[type] );

    // Add the tags to the URL
    char loggingURL[strlen( _fullServiceURL )+LOG_MAX_TAG_DESC_LEN+2];
    strcpy( loggingURL, _fullServiceURL );
    strcat_P( loggingURL, _logTagDescriptions[tag] );
    strcat_P( loggingURL, PSTR("/") );
   
    if( _settings->serialModeOn && _settings->level == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix( DETAIL_LOG, STATUS_TAG );
        Serial.print( PSTR("(Logger) Logging to: ") );
        Serial.println (loggingURL );
    }

    // Build JSON
    DynamicJsonDocument jsonLog(LOG_SERVICE_JSON_SIZE);

    jsonLog[F("localtime")] = millis();

    char shortened[LOG_MAX_MESSAGE_LEN];
    strncpy( shortened, message, LOG_MAX_MESSAGE_LEN );        // Truncate if too long

    jsonLog[F("message")] = shortened;

    JsonObject Device = jsonLog.createNestedObject( F("Device") );

        JsonObject Device_Hardware = Device.createNestedObject( F("Hardware") );
            Device_Hardware[F("Board")] = FPSTR(flag_BOARD);
            String tempMAC = WiFi.macAddress();
            Device_Hardware[F("MAC")] = tempMAC.c_str();

        JsonObject Device_Env = Device.createNestedObject( F("Env") );
            Device_Env[F("Name")] = FPSTR(flag_DEVICE_NAME);
            Device_Env[F("Code")] = FPSTR(flag_DEVICE_CODE);
            Device_Env[F("Build")] = FPSTR(flag_BUILD_ENV);
            Device_Env[F("Tag")] = FPSTR(flag_BUILD_TAG);
            Device_Env[F("Heap")] = system_get_free_heap_size();

        JsonObject Device_Network = Device.createNestedObject( F("Network") );
            String tempIP = WiFi.localIP().toString(); Device_Network[F("IPAddress")] = tempIP.c_str();
            String tempSSID = WiFi.SSID();
            Device_Network[F("SSID")] = tempSSID.c_str();

    // Serialize for serial only if needed
    if( _settings->serialModeOn && _settings->level == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix( DETAIL_LOG, STATUS_TAG );
        Serial.print( PSTR("(Logger) Log (JSON): ") );
        serializeJson( jsonLog, Serial ); 
    }

    // Serialize for Loggly
    size_t jsonSize = measureJson( jsonLog );
    char jsonMessage[jsonSize+1]; 
    if( serializeJson( jsonLog, jsonMessage, sizeof(jsonMessage) ) != jsonSize ) {
        LogPrefix( CRITICAL_LOG, STATUS_TAG );
        Serial.print( PSTR("(Logger) Logging to service: JSON Error ") );
    };

    // Start the connection
    HTTPClient http;

    if( !http.begin( *_client, loggingURL ) ) {
        if( _settings->serialModeOn && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix( CRITICAL_LOG, STATUS_TAG );
            Serial.print( PSTR("(Logger) Logging to servce: HTTP Client Error ") );
            http.end();
        }
    }

    // Post the message to Loggly
    http.setUserAgent( FPSTR(flag_DEVICE_CODE) );
    http.addHeader( PSTR("Content-Type"), PSTR("application/json") );
    int httpCode = http.POST( jsonMessage );
    http.end();

    // Check for confrimation
    if( httpCode == HTTP_CODE_OK ) {
        if( _settings->serialModeOn && _settings->level == LOGGING_LEVEL_VERBOSE ) {
            LogPrefix( DETAIL_LOG, STATUS_TAG );
            Serial.println( PSTR("(Logger) Logging to servce: SUCCESS ") );
        }
    }
    else {
        if( _settings->serialModeOn && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix( CRITICAL_LOG, STATUS_TAG );
            Serial.print( PSTR("(Logger) Logging to servce: ERROR ") );
            if( httpCode < 0 ) Serial.println( http.errorToString( httpCode ).c_str() );
            else Serial.println( httpCode );
        }
    }

#endif

}


// Send tick to Loggly Service
void LogClient::HandleTick( ) {

#ifndef NO_LOGGING

    if( WiFi.status() != WL_CONNECTED ) return;             // TODO - Use isconnected? Elsewhere too

    if( _settings->serialModeOn && _settings->level >= LOGGING_LEVEL_NORMAL ) {
        LogPrefix( NORMAL_LOG, STATUS_TAG );
        Serial.println( PSTR("(Logger) Logging a tick") ); 
    }

    char thistype[LOG_MAX_TYPE_DESC_LEN];
    strcpy_P( thistype, _logTypeDescriptions[NORMAL_LOG] );

    char loggingURL[strlen( _fullServiceURL )+LOG_MAX_TAG_DESC_LEN+2];
    strcpy( loggingURL,_fullServiceURL );
    strcat_P( loggingURL, _logTagDescriptions[STATUS_TAG] );
    strcat_P( loggingURL, PSTR("/") );

    // Build JSON
    DynamicJsonDocument jsonLog(LOG_TICK_JSON_SIZE);

    jsonLog[F("localtime")] = millis();

    JsonObject Device = jsonLog.createNestedObject( F("Device") );

        JsonObject Device_Hardware = Device.createNestedObject( F("Hardware") );
            Device_Hardware[F("Board")] = FPSTR(flag_BOARD);
            Device_Hardware[F("MAC")] = WiFi.macAddress();

        JsonObject Device_Env = Device.createNestedObject( F("Env") );
            Device_Env[F("Name")] = FPSTR(flag_DEVICE_NAME);
            Device_Env[F("Code")] = FPSTR(flag_DEVICE_CODE);
            Device_Env[F("Build")] = FPSTR(flag_BUILD_ENV);
            Device_Env[F("Tag")] = FPSTR(flag_BUILD_TAG);
            Device_Env[F("Heap")] = system_get_free_heap_size();

        JsonObject Device_Network = Device.createNestedObject( F("Network") );
            String tempIP = WiFi.localIP().toString();
            Device_Network[F("IPAddress")] = tempIP.c_str();
            String tempSSID = WiFi.SSID(); Device_Network[F("SSID")] = tempSSID.c_str();

    // Serialize for Loggly
    size_t jsonSize = measureJson( jsonLog );
    char jsonMessage[jsonSize+1]; 
    if( serializeJson( jsonLog, jsonMessage, sizeof(jsonMessage) ) != jsonSize ) {
        LogPrefix( CRITICAL_LOG, STATUS_TAG );
        Serial.print( PSTR("(Logger) Logging tick: JSON Error ") );
        return;
    };

    // Start the connection
    HTTPClient http;

    if( !http.begin( *_client, loggingURL ) ) {
        if( _settings->serialModeOn && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix( CRITICAL_LOG, STATUS_TAG );
            Serial.print( PSTR("(Logger) Logging tick: HTTP Client Error ") );
            http.end();
            return;
        }
    }
    
    // Send to Loggly
    http.setUserAgent( FPSTR(flag_DEVICE_CODE) );
    http.addHeader( PSTR("Content-Type"), PSTR("application/json") );
    int httpCode = http.POST( jsonMessage );
    http.end();

    // Check for success
    if( httpCode == HTTP_CODE_OK ) {
        if( _settings->serialModeOn && _settings->level == LOGGING_LEVEL_VERBOSE ) {
            LogPrefix( DETAIL_LOG, STATUS_TAG );
            Serial.println( PSTR("(Logger) Logging tick: SUCCESS ") );
        }
    }
    else {
        if( _settings->serialModeOn && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix( CRITICAL_LOG, STATUS_TAG) ;
            Serial.print( PSTR("(Logger) Logging to tick: ERROR ") );
            if( httpCode < 0 ) Serial.println( http.errorToString( httpCode ).c_str() );
            else Serial.println( httpCode );
        }
    }

#endif

}


LogClient logger;       // Create the global instance
