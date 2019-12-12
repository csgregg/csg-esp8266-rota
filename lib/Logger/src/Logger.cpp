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

*/


#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "IOTDevice.h"
#include "Logger.h"


// Public

// Creator - not used
LogClient::LogClient() {

}


// Sets up logger - needs to be followed by setMode() to actually start
void LogClient::begin( WiFiClient &client, const long baud, const String &service, const String &key, const String &tags ) {

#ifndef NO_DEBUG

    _client = &client;

    _ServiceURL = PSTR("http://") + service + PSTR("/") + key + PSTR("/tag/") + tags + PSTR("/");
    Serial.println(_ServiceURL);

    Serial.begin(baud);

    Serial.println(F("\n\nLOG: (Logger) Starting Logging\n"));

#endif
    
}


// Sets logging mode
void LogClient::setMode( const bool modeSerial, const bool modeService, const t_logging_level level ){

#ifndef NO_DEBUG

    _serialOn = modeSerial;
    _serviceOn = modeService;
    _logginglevel = level;

    setTypeTag(LOG_NORMAL, TAG_STATUS);
    PGM_P format1 = PSTR("(Logger) Logging set at level: %i");
    logger.printf( format1, _logginglevel );

    if(_serviceOn) LOG(F("(Logger) Logging Service: ON"));
    else LOG(F("(Logger) Logging Service: OFF"));

#endif

}


// Main log function - char[]
void LogClient::println( const t_log_type type, const t_log_tag tag, const char * message ) {

#ifndef NO_DEBUG

    if( _logginglevel == LOGGING_OFF ) return;
    if( _logginglevel == LOGGING_LEVEL_CRITICAL && type != LOG_CRITICAL ) return;
    if( _logginglevel == LOGGING_LEVEL_NORMAL && type == LOG_DETAIL ) return;
    
    if( _serialOn ) LogToSerial(type, tag, message);
    if( _serviceOn ) LogToService(type, tag, message);

#endif

}


// Overload - with context
void LogClient::println(const t_log_type type, const t_log_tag tag, const char * message, const char * file, const char * func_P, const int line ) {

#ifndef NO_DEBUG

    char func[MAX_MESSAGE_LEN];

    strcpy_P(func, func_P);             // __FUNC__ is held in Flash so handle appropriately

    if( _logginglevel == LOGGING_LEVEL_VERBOSE ) {

        PGM_P format = PSTR("(Context: %s %s %i) %s");
        size_t contextsize =  ( strlen(format) - 8 ) + strlen(file) + strlen(func);

        char shortened[MAX_MESSAGE_LEN];
        strncpy( shortened, message, MAX_MESSAGE_LEN - contextsize );        // Truncate if too long
        shortened[(MAX_MESSAGE_LEN - contextsize)-1]=0;

        char str[MAX_MESSAGE_LEN];
        sprintf(str, format, file, func, line, shortened);
        str[MAX_MESSAGE_LEN-1] = 0;
        println(type, tag, str);
    }
    else println(type, tag, message);

#endif

}


// Overload println() - char
void LogClient::println( const t_log_type type, const t_log_tag tag, char c ) {

#ifndef NO_DEBUG

    char c_str[2];
    c_str[0] = c;
    c_str[1] = 0;

    println(type, tag, c_str);

#endif

}


// Overload println() - char with context
void LogClient::println( const t_log_type type, const t_log_tag tag, char c, const char * file, const char * func_P, const int line ) {

#ifndef NO_DEBUG

    char c_str[2];
    c_str[0] = c;
    c_str[1] = 0;
  
    println(type, tag, c_str, file, func_P, line);

#endif

}


// Overload println() - string
void LogClient::println( const t_log_type type, const t_log_tag tag, const String &s ) {

#ifndef NO_DEBUG

    println( type, tag, s.c_str() );

#endif

}


// Overload println() - string with context
void LogClient::println( const t_log_type type, const t_log_tag tag, const String &s, const char * file, const char * func_P, const int line ) {

#ifndef NO_DEBUG

    println( type, tag, s.c_str(), file, func_P, line  );

#endif

}


// Sets up Tag and Type for printf() function
void LogClient::setTypeTag( const t_log_type type, const t_log_tag tag ){

#ifndef NO_DEBUG
    
    _lasttype = type;
    _lasttag = tag;

#endif

}


// Formatted log function - needs to be preceded by setTagType()
void LogClient::printf( const char * format, ... ) {

#ifndef NO_DEBUG

    va_list arg;
    va_start(arg, format);
    char temp[MAX_MESSAGE_LEN];
    char* buffer = temp;

    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            println(LOG_CRITICAL, TAG_DEBUG, F("(Logger) LogClient: Buffer error"));
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }

    if (buffer != temp) {
        delete[] buffer;
    }

    println(_lasttype, _lasttag, temp);

#endif

}


// Protected:

// Create and log prefix - needs to be followed by message using Serial.println()
void LogClient::LogPrefix( const t_log_type type, const t_log_tag tag ){

#ifndef NO_DEBUG

    if( _logginglevel == LOGGING_LEVEL_VERBOSE) {
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
void LogClient::LogToSerial( t_log_type type, t_log_tag tag, const char * message ){

#ifndef NO_DEBUG

    char shortened[MAX_MESSAGE_LEN+1];

    strncpy( shortened, message, MAX_MESSAGE_LEN );        // Truncate if too long

    LogPrefix(type, tag);
    Serial.println(shortened);

#endif

}


// Log message to Loggly Service
void LogClient::LogToService( const t_log_type type, const t_log_tag tag, const char * message ){

#ifndef NO_DEBUG

    char thistag[strlen(c_log_tag_descript[tag])];
    strcpy(thistag, c_log_tag_descript[tag]);

    char thistype[strlen(c_log_type_descript[type])];
    strcpy(thistype, c_log_type_descript[type]);

    String loggingURL = _ServiceURL + String(thistag) + "/";
   
    if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print(F("(Logger) Logging to: "));
        Serial.println(loggingURL);
    }

    // Build JSON
    // Use https://arduinojson.org/v6/assistant/
    
    const size_t capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 640;
    DynamicJsonDocument jsonLog(capacity);

    jsonLog["localtime"] = millis();

    char shortened[MAX_MESSAGE_LEN];

    strncpy( shortened, message, MAX_MESSAGE_LEN );        // Truncate if too long
    shortened[MAX_MESSAGE_LEN-1]=0;

    jsonLog["message"] = shortened;

    JsonObject Device = jsonLog.createNestedObject("Device");

        JsonObject Device_Hardware = Device.createNestedObject("Hardware");
        Device_Hardware["platform"] = device_getBuildFlag(flag_PLATFORM);
        Device_Hardware["board"] = device_getBuildFlag(flag_BUILD_TAG);
        Device_Hardware["framework"] = device_getBuildFlag(flag_FRAMEWORK);
        String tempMAC = WiFi.macAddress(); Device_Hardware["MAC"] =  tempMAC.c_str();

        JsonObject Device_Env = Device.createNestedObject("Env");
        Device_Env["Name"] = device_getBuildFlag(flag_DEVICE_NAME);
        Device_Env["Code"] = device_getBuildFlag(flag_DEVICE_CODE);
        Device_Env["Build"] = device_getBuildFlag(flag_BUILD_ENV);
        Device_Env["Heap"] = system_get_free_heap_size();

        JsonObject Device_Network = Device.createNestedObject("Network");

        String tempIP = WiFi.localIP().toString();
        Device_Network["IPAddress"] = tempIP.c_str();

        String tempSSID = WiFi.SSID();
        Device_Network["SSID"] = tempSSID.c_str();

    String jsonMessage;

    serializeJson(jsonLog, jsonMessage);

    if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print(F("(Logger) Log (JSON): "));
        Serial.println(jsonMessage); 
    }


    // Start the connection
    
    HTTPClient http;

    http.begin(*_client, loggingURL);

    http.setUserAgent(F("ESP8266-http-logger"));
    http.addHeader(F("Content-Type"), F("content-type:text/plain"));

    int httpCode = http.POST(jsonMessage);
    String payload = http.getString();
    http.end();

    if( httpCode == HTTP_CODE_OK ) {
        if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
            LogPrefix(LOG_DETAIL, TAG_STATUS);
            Serial.println(F("(Logger) Logging to servce: SUCCESS "));
        }
    }
    else {
        if( _serialOn && _logginglevel > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix(LOG_CRITICAL, TAG_STATUS);
            if( httpCode < 0 ) Serial.printf("(Logger) Logging to servce: ERROR %s\n", http.errorToString(httpCode).c_str());
            else Serial.printf("(Logger) Logging to servce: ERROR %i\n", httpCode);
        }
    }

#endif

}



// Create the global logger instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif