/* Logger Library

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


TODO - Add function to check in and regsiter

*/


#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "Logger.h"
#include "Device.h"



void ICACHE_FLASH_ATTR LogSettings::setDefaults() {
    serialBaud = flag_MONITOR_SPEED;
    strcpy_P(serviceURL,flag_LOGGER_SERVICE);
    strcpy_P(serviceKey,flag_LOGGER_SERVICE_KEY);
    serialMode = flag_LOGGER_AS_SERIAL;
    serviceMode = flag_LOGGER_AS_SERVICE;
    strcpy_P(globalTags,flag_LOGGER_GLOBAL_TAGS);
    level = logLevel(flag_LOGGER_LEVEL);
}



// Public:

// Sets up logger
void ICACHE_FLASH_ATTR LogClient::begin( LogSettings &settings ) {

     _settings = &settings;

#ifndef NO_LOGGING

    if( _settings->serviceMode ) {
        _FullServiceURL = PSTR("http://");
        _FullServiceURL =+ _settings->serviceURL;
        _FullServiceURL =+ PSTR("/");
        _FullServiceURL =+ _settings->serviceKey;
        _FullServiceURL =+ PSTR("/tag/");
        _FullServiceURL =+ _settings->globalTags;
        _FullServiceURL =+ PSTR("/");
    }

    if( _settings->serialMode ) {
        Serial.begin(_settings->serialBaud);             
        Serial.println(F("\nLOG: (Logger) Starting Logging"));
    }

    PGM_P format1 = PSTR("(Logger) Logging set at level: %i");
    logger.printf( LOG_HIGH, TAG_STATUS, format1,_settings->level );

    if( _settings->serviceMode ) LOG_HIGH(F("(Logger) Logging Service: ON"));
    else LOG_HIGH(F("(Logger) Logging Service: OFF"));

#endif

}
void ICACHE_FLASH_ATTR LogClient::begin( WiFiClient &client, LogSettings &settings ) {

    _client = &client;
    begin(settings);
   
}


// TODO Need to add print functions for flashstringhelper
// TODO Add int function
// TODO Need to add return JSON header only

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

        char func[MAX_MESSAGE_LEN];

        strcpy_P(func, func_P);             // __FUNC__ is held in Flash so handle appropriately

        PGM_P format = PSTR("(Context: %s %s %i) %s");
        size_t contextsize =  ( strlen(format) - 8 ) + strlen(file) + strlen(func);

        char shortened[MAX_MESSAGE_LEN];
        strncpy( shortened, message, MAX_MESSAGE_LEN - contextsize );        // Truncate if too long
        shortened[(MAX_MESSAGE_LEN - contextsize)-1] = 0;

        char str[MAX_MESSAGE_LEN];
        sprintf(str, format, file, func, line, shortened);
        str[MAX_MESSAGE_LEN-1] = 0;
        println(type, tag, str);
    }
    else println(type, tag, message);

#endif

}



// Overload println() - int
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, int i ) {

#ifndef NO_LOGGING

    char msg[sizeof(int)*3];
    sprintf(msg,"%i",i);
    println(type, tag, msg);

#endif

}
// Overload println() - int with context
void ICACHE_FLASH_ATTR LogClient::println( const logType type, const logTag tag, int i, const char * file, const char * func_P, const int line ) {

#ifndef NO_LOGGING

    char msg[sizeof(int)*3];
    sprintf(msg,"%i",i);
    println(type, tag, msg, file, func_P, line);

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



// Formatted log function - needs to be preceded by setTagType()
void ICACHE_FLASH_ATTR LogClient::printf( const logType type, const logTag tag, const char * format, ... ) {

#ifndef NO_LOGGING

    va_list arg;
    va_start(arg, format);
    char temp[MAX_MESSAGE_LEN];
    char* buffer = temp;

    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            println(LOG_CRITICAL, TAG_STATUS, F("(Logger) LogClient: Buffer error"));
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



void ICACHE_FLASH_ATTR LogClient::printFlag(const logType type, const logTag tag, const char* name, const char* flag) {

#ifndef NO_LOGGING

    char buffer[MAX_MESSAGE_LEN];
    sprintf(buffer, PSTR("(Build) %s: %s"), name, FPSTR(flag));

    println(type, tag, buffer);

#endif
}

void ICACHE_FLASH_ATTR LogClient::printFlag(const logType type, const logTag tag, const char* name, const bool flag) {

#ifndef NO_LOGGING

    char buffer[MAX_MESSAGE_LEN];
    sprintf(buffer, PSTR("(Build) %s: %i"), name, flag);

    println(type, tag, buffer);

#endif
}

void ICACHE_FLASH_ATTR LogClient::printFlag(const logType type, const logTag tag, const char* name, const uint flag) {

#ifndef NO_LOGGING

    char buffer[MAX_MESSAGE_LEN];
    sprintf(buffer, PSTR("(Build) %s: %i"), name, flag);

    println(type, tag, buffer);

#endif
}



// Protected:

// Create and log prefix - needs to be followed by message using Serial.println()
void ICACHE_FLASH_ATTR LogClient::LogPrefix( const logType type, const logTag tag ){

#ifndef NO_LOGGING

    if( _settings->level == LOGGING_LEVEL_VERBOSE) {
        PGM_P format = PSTR("LOG: %s: %s - Millis: %li, Heap: %i - ");
        Serial.printf(format, c_log_tag_descript[tag], c_log_type_descript[type], millis(), system_get_free_heap_size());
    }
    else {
        PGM_P format = PSTR("LOG: %s: %s - ");
        Serial.printf(format, c_log_tag_descript[tag], c_log_type_descript[type]);
    }

#endif

}


// Log message to serial
void ICACHE_FLASH_ATTR LogClient::LogToSerial( logType type, logTag tag, const char * message ){

#ifndef NO_LOGGING

    char shortened[MAX_MESSAGE_LEN+1];

    strncpy( shortened, message, MAX_MESSAGE_LEN );        // Truncate if too long

    LogPrefix(type, tag);
    Serial.println(shortened);

#endif

}


// Log message to Loggly Service
void ICACHE_FLASH_ATTR LogClient::LogToService( const logType type, const logTag tag, const char * message ){

#ifndef NO_LOGGING

    if( !_client->connected() ) return;             // TODO - better error handling needed

    char thistag[strlen(c_log_tag_descript[tag])];
    strcpy(thistag, c_log_tag_descript[tag]);

    char thistype[strlen(c_log_type_descript[type])];
    strcpy(thistype, c_log_type_descript[type]);

    String loggingURL = _FullServiceURL + String(thistag) + "/";
   
    if( _settings->serialMode && _settings->level == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print(F("(Logger) Logging to: "));
        Serial.println(loggingURL);
    }

    // Build JSON
    // Use https://arduinojson.org/v6/assistant/
    
    const size_t capacity = 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 509;
    DynamicJsonDocument jsonLog(capacity);

    jsonLog[F("localtime")] = millis();

    char shortened[MAX_MESSAGE_LEN];

    strncpy( shortened, message, MAX_MESSAGE_LEN );        // Truncate if too long
    shortened[MAX_MESSAGE_LEN-1]=0;

    jsonLog[F("message")] = shortened;

    JsonObject Device = jsonLog.createNestedObject("Device");

        JsonObject Device_Hardware = Device.createNestedObject(F("Hardware"));
        Device_Hardware[F("Platform")] = FPSTR(flag_PLATFORM);     // TODO - Remove
        Device_Hardware[F("Board")] = FPSTR(flag_BOARD);
        Device_Hardware[F("Framework")] = FPSTR(flag_FRAMEWORK);     // TODO - Remove
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

    String jsonMessage;

    serializeJson(jsonLog, jsonMessage);

    if( _settings->serialMode && _settings->level == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print(F("(Logger) Log (JSON): "));
        Serial.println(jsonMessage); 
    }


    // Start the connection
    
    HTTPClient http;

    http.begin(*_client, loggingURL);           // TODO - add some error handling

    http.setUserAgent(F("ESP8266-http-logger"));
    http.addHeader(F("Content-Type"), F("content-type:text/plain"));

    int httpCode = http.POST(jsonMessage);
    String payload = http.getString();
    http.end();

    if( httpCode == HTTP_CODE_OK ) {
        if( _settings->serialMode && _settings->level == LOGGING_LEVEL_VERBOSE ) {
            LogPrefix(LOG_DETAIL, TAG_STATUS);
            Serial.println(F("(Logger) Logging to servce: SUCCESS "));
        }
    }
    else {
        if( _settings->serialMode && _settings->level > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix(LOG_CRITICAL, TAG_STATUS);
            Serial.print(F("(Logger) Logging to servce: ERROR "));
            if( httpCode < 0 ) Serial.println( http.errorToString(httpCode).c_str() );
            else Serial.println( httpCode );
        }
    }

#endif

}


// Create the global logger instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif