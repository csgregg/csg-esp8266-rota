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
void LogClient::begin( HTTPClient &http, WiFiClient &client ) {

#ifndef NO_DEBUG

    _http = &http;
    _client = &client;

    Serial.begin(L_MONITOR_BAUD);

    Serial.println(F("\r\n\r\nLOG: (Logger) Starting Logging\r\n"));

#endif
    
}


// Sets logging mode
void LogClient::setMode( const bool modeSerial, const bool modeService, const t_logging_level level ){

#ifndef NO_DEBUG

    _serialOn = modeSerial;
    _serviceOn = modeService;
    _logginglevel = level;

    setTypeTag(LOG_NORMAL, TAG_STATUS);
    static const char msg1[] PROGMEM = "(Logger) Logging set at level:" ESCAPEQUOTE(LOG_LEVEL);
    LOG( msg1 );

    if(_serviceOn) logger.println(LOG_NORMAL, TAG_STATUS, F("(Logger) Logging Service: ON"));
    else logger.println(LOG_NORMAL, TAG_STATUS, F("(Logger) Logging Service: OFF"));

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

    char shortened[MAX_MESSAGE_LEN];
    char func[MAX_MESSAGE_LEN];

    strcpy_P(func, func_P);             // __FUNC__ is held in PROGMEM so handle appropriately

    const char format[] = "(Context: %s %s %i) %s";

    strncpy( shortened, message, MAX_MESSAGE_LEN - ( strlen(format) + strlen(file) + strlen(func) ) );        // Truncate if too long
    
    if( _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        char str[MAX_MESSAGE_LEN];
        sprintf(str, format, file, func, line, shortened);
        str[MAX_MESSAGE_LEN-1] = 0;
        println(type, tag, str);
    }
    else println(type, tag, shortened);

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

    if( _logginglevel == LOGGING_LEVEL_VERBOSE) Serial.printf("LOG: %s: %s - Millis: %li, Heap: %i - ", c_log_tag_descript[tag], c_log_type_descript[type], millis(), system_get_free_heap_size());
    else Serial.printf("LOG: %s: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);

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

    static char url[] PROGMEM = "http://" ESCAPEQUOTE(LOGGING_SERVICE) "/" ESCAPEQUOTE(LOGGING_SERVICE_KEY) "/tag/" ESCAPEQUOTE(LOGGING_GLOBAL_TAGS) ",";

    char loggingURL[strlen(url) + strlen(thistag) + 2];
    strcpy( loggingURL, url );

    strcat(loggingURL, thistag); 
    strcat(loggingURL, "/");  
   
    if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print(F("(Logger) Logging to: "));
        Serial.println(loggingURL);
    }

    // Build JSON
    // Use https://arduinojson.org/v6/assistant/

    const size_t capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 454;
    DynamicJsonDocument jsonLog(capacity);

    jsonLog["localtime"] = millis();

    char shortened[MAX_MESSAGE_LEN+1];

    strncpy( shortened, message, MAX_MESSAGE_LEN );        // Truncate if too long

    jsonLog["message"] = shortened;

    JsonObject Device = jsonLog.createNestedObject("Device");

        JsonObject Device_Hardware = Device.createNestedObject("Hardware");
        Device_Hardware["platform"] = FPSTR(STR_PLATFORM_P);
        Device_Hardware["board"] = FPSTR(STR_BOARD_P);
        Device_Hardware["framework"] = FPSTR(STR_FRAMEWORK_P);

        String tempMAC = WiFi.macAddress();
        Device_Hardware["MAC"] =  tempMAC.c_str();

        JsonObject Device_Env = Device.createNestedObject("Env");
        Device_Env["Name"] = FPSTR(STR_DEVICE_NAME_P);
        Device_Env["Code"] = FPSTR(STR_DEVICE_CODE_P);
        Device_Env["Build"] = FPSTR(STR_BUILD_ENV_P);
        Device_Env["Heap"] = system_get_free_heap_size();

        JsonObject Device_Network = Device.createNestedObject("Network");

        String tempIP = WiFi.localIP().toString();
        Device_Network["IPAddress"] = tempIP.c_str();

        String tempSSID = WiFi.SSID();
        Device_Network["SSID"] = tempSSID.c_str();

    int jsonSize = measureJson(jsonLog)+1;
    char jsonMessage[jsonSize];

    serializeJson(jsonLog, jsonMessage, jsonSize);

    if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print(F("(Logger) Log (JSON): "));
        Serial.println(jsonMessage); 
    }

    // Start the connection
    _http->begin(*_client, loggingURL);
    _http->addHeader(F("Content-Type"), F("content-type:text/plain"));

    int httpCode = _http->POST((uint8_t *) &jsonMessage, jsonSize);

    _http->end();

    if( httpCode == HTTP_CODE_OK ) {
        if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
            LogPrefix(LOG_DETAIL, TAG_STATUS);
            Serial.printf("(Logger) Logging to servce: SUCCESS %i", httpCode);
            Serial.println();
        }
        return;
    }
    else {
        if( _serialOn && _logginglevel > LOGGING_LEVEL_CRITICAL ) {
            LogPrefix(LOG_CRITICAL, TAG_STATUS);
            Serial.printf("(Logger) Logging to servce: ERROR %i", httpCode);
            Serial.println(); 
        }
        return;
    }

#endif

}



// Create the global logger instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif