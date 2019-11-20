/* Library Info and License
*/


#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "iot_device.h"
#include "logger.h"


// Public

// Creator - not used
LogClient::LogClient() {

}


// Sets up logger - needs to be followed by setMode() to actually start
void LogClient::begin( HTTPClient &http, WiFiClient &client ) {

#ifndef NO_DEBUG

    _http = &http;
    _client = &client;

    Serial.begin(device.monitorBaud);

    Serial.println("\r\n\r\nLOG: (Logger) Starting Logging\r\n");

#endif
    
}


// Sets logging mode
void LogClient::setMode( bool modeSerial, bool modeService, t_logging_level level ){

#ifndef NO_DEBUG

    _serialOn = modeSerial;
    _serviceOn = modeService;
    _logginglevel = level;

    setTypeTag(LOG_NORMAL, TAG_STATUS);
    printf("(Logger) Logging set at level: %i", device.loggingLevel);

    if(_serviceOn) logger.println(LOG_NORMAL, TAG_STATUS, "(Logger) Logging Service: ON");
    else logger.println(LOG_NORMAL, TAG_STATUS, "(Logger) Logging Service: OFF");

#endif

}


// Main log function
void LogClient::println( t_log_type type, t_log_tag tag, const String &s ) {

#ifndef NO_DEBUG

    if( _logginglevel == LOGGING_OFF ) return;
    if( _logginglevel == LOGGING_LEVEL_CRITICAL && type != LOG_CRITICAL ) return;
    if( _logginglevel == LOGGING_LEVEL_NORMAL && type == LOG_DETAIL ) return;
    
    if( _serialOn ) LogToSerial(type, tag, s);
    if( _serviceOn ) LogToService(type, tag, s);

#endif

}


// Overload println() - with code context
void LogClient::println( t_log_type type, t_log_tag tag, const String &s, const String &file, const String &func, const int line ){

#ifndef NO_DEBUG

    String str;
    
    if( _logginglevel == LOGGING_LEVEL_VERBOSE ) str = "(Context: " + file + " " + func + " " + String(line) + ") " + s;
    else str = s;

    println(type, tag, str);

#endif

}


// Overload println() - char[]
void LogClient::println( t_log_type type, t_log_tag tag, const char c[] ) {

#ifndef NO_DEBUG

    String s(c);

    println(type, tag, s);

#endif

}


// Overload println() - char
void LogClient::println( t_log_type type, t_log_tag tag, char c ) {

#ifndef NO_DEBUG

    String s(c);

    println(type, tag, s);

#endif

}


// Sets up Tag and Type for printf() function
void LogClient::setTypeTag( t_log_type type, t_log_tag tag ){

#ifndef NO_DEBUG
    
    _lasttype = type;
    _lasttag = tag;

#endif

}


// Formatted log function - needs to be preceded by setTagType()
void LogClient::printf( const char *format, ... ) {

#ifndef NO_DEBUG

    va_list arg;
    va_start(arg, format);
    char temp[64];
    char* buffer = temp;

    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            println(LOG_CRITICAL, TAG_DEBUG, "(Logger) LogClient:: Buffer error");
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }

    if (buffer != temp) {
        delete[] buffer;
    }

    String s(temp);

    println(_lasttype, _lasttag, s);

#endif

}


// Protected:

// Create and log prefix - needs to be followed by message using Serial.println()
void LogClient::LogPrefix( t_log_type type, t_log_tag tag ){

#ifndef NO_DEBUG

    if( _logginglevel == LOGGING_LEVEL_VERBOSE) Serial.printf("LOG: %s: %s - Millis: %li, Heap: %i - ", c_log_tag_descript[tag], c_log_type_descript[type], millis(), system_get_free_heap_size());
    else Serial.printf("LOG: %s: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);

#endif

}


// Log message to serial
void LogClient::LogToSerial( t_log_type type, t_log_tag tag, String message ){

#ifndef NO_DEBUG

    LogPrefix(type, tag);
    Serial.println(message);

#endif

}


// Log message to Loggly Service
void LogClient::LogToService( t_log_type type, t_log_tag tag, String message ){

#ifndef NO_DEBUG

    String _tag(c_log_tag_descript[tag]);
    String _type(c_log_type_descript[type]);

    _tag = "," + _tag;

    String loggingServiceRequestURL = "http://" + device.loggingService + "/" + device.loggingServiceKey + "/tag/" + device.loggingGlobalTags + _tag  + "/";

    if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print("(Logger) Logging to: ");
        Serial.println(loggingServiceRequestURL);
    }

    // Build JSON
    // Use https://arduinojson.org/v6/assistant/

    const size_t capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 454;
    DynamicJsonDocument jsonLog(capacity);
    String jsonMessage;

    jsonLog["localtime"] = millis();

    if(message.length() > MAX_MESSAGE_LEN ) message = message.substring(0, MAX_MESSAGE_LEN);

    jsonLog["message"] = message;

    JsonObject Device = jsonLog.createNestedObject("Device");

        JsonObject Device_Hardware = Device.createNestedObject("Hardware");
        Device_Hardware["platform"] = device.platform.c_str();
        Device_Hardware["board"] = device.board.c_str();
        Device_Hardware["framework"] = device.framework.c_str();

        String tempMAC = WiFi.macAddress();
        Device_Hardware["MAC"] = tempMAC.c_str();

        JsonObject Device_Env = Device.createNestedObject("Env");
        Device_Env["Name"] = device.deviceName.c_str();
        Device_Env["Code"] = device.deviceCode.c_str();
        Device_Env["Build"] = device.buildTag.c_str();

        uint32_t free = system_get_free_heap_size();    // get free ram 
        Device_Env["Heap"] = free;

        JsonObject Device_Network = Device.createNestedObject("Network");

        String tempIP = WiFi.localIP().toString();
        Device_Network["IPAddress"] = tempIP.c_str();

        String tempSSID = WiFi.SSID();
        Device_Network["SSID"] = tempSSID.c_str();

    serializeJson(jsonLog, jsonMessage);

    if( _serialOn && _logginglevel == LOGGING_LEVEL_VERBOSE ) {
        LogPrefix(LOG_DETAIL, TAG_STATUS);
        Serial.print("(Logger) Log (JSON): ");
        Serial.println(jsonMessage); 
    }

    // Start the connection
    _http->begin(*_client, loggingServiceRequestURL);
    _http->addHeader("Content-Type", "content-type:text/plain");

    int httpCode = _http->POST(jsonMessage);

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


// Create the global logger

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif