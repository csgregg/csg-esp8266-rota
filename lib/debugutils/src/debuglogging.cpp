/* Library Info and License
*/


#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "iot_device.h"
#include "debuglogging.h"


LogClient::LogClient() {

}


void LogClient::begin( HTTPClient &http, WiFiClient &client ) {
    _http = &http;
    _client = &client;

    Serial.begin(device.monitorBaud);
    Serial.println();
    LogPrefix(LOG_INFO, TAG_STATUS);
    Serial.println("Starting Logging");
    Serial.println();
}


void LogClient::setMode( bool modeSerial, bool modeService, t_logging_level level ){
    _serialOn = modeSerial;
    _serviceOn = modeService;
    _logginglevel = level;

    setTypeTag(LOG_INFO, TAG_STATUS);
    printf("Logging set at level: %i", device.loggingLevel);

    if(_serviceOn) logger.println(LOG_INFO, TAG_STATUS, "Logging Service: ON");
    else logger.println(LOG_INFO, TAG_STATUS, "Logging Service: OFF");
}


void LogClient::setTypeTag(t_log_type type, t_log_tag tag){
    _lasttype = type;
    _lasttag = tag;
}


void LogClient::printf(const char *format, ...) {

    va_list arg;
    va_start(arg, format);
    char temp[64];
    char* buffer = temp;

    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            println(LOG_INFO, TAG_STATUS, "LogClient:: Buffer error");
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
}


void LogClient::println(t_log_type type, t_log_tag tag, const String &s) {

    if( _logginglevel == LOGGING_OFF ) return;
    if( _logginglevel == LOGGING_LEVEL_LOW && type != LOG_CRITICAL ) return;
    if( _logginglevel == LOGGING_LEVEL_MED && type == LOG_INFO ) return;
    
    if( _serialOn ) LogToSerial(type, tag, s);
    if( _serviceOn ) LogToService(type, tag, s);

}

void LogClient::println(t_log_type type, t_log_tag tag, const char c[]) {

    String s(c);

    println(type, tag, s);

}

void LogClient::println(t_log_type type, t_log_tag tag, char c) {

    String s(c);

    println(type, tag, s);

}


void LogClient::LogPrefix(t_log_type type, t_log_tag tag){
    uint32_t free = system_get_free_heap_size(); // get free ram
    if( _logginglevel == LOGGING_LEVEL_HIGH ) Serial.printf("LOG: %s :: %s - Heap: %i, Millis: %li - ", c_log_tag_descript[tag], c_log_type_descript[type], free, millis());
    else Serial.printf("LOG: %s :: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);
}


void LogClient::LogToSerial(t_log_type type, t_log_tag tag, String message){
    LogPrefix(type, tag);
    Serial.println(message);
}


void LogClient::LogToService(t_log_type type, t_log_tag tag, String message){

    String _tag(c_log_tag_descript[tag]);
    String _type(c_log_type_descript[type]);

    _tag = "," + _tag;

    String loggingServiceRequestURL = "http://" + device.loggingService + "/" + device.loggingServiceKey + "/tag/" + device.loggingGlobalTags + _tag  + "/";

    if( _serialOn && _logginglevel == LOGGING_LEVEL_HIGH ) {
        LogPrefix(LOG_INFO, TAG_STATUS);
        Serial.print("Logging to: ");
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

        uint32_t free = system_get_free_heap_size(); // get free ram 
        Device_Env["Heap"] = free;

        JsonObject Device_Network = Device.createNestedObject("Network");

        String tempIP = WiFi.localIP().toString();
        Device_Network["IPAddress"] = tempIP.c_str();

        String tempSSID = WiFi.SSID();
        Device_Network["SSID"] = tempSSID.c_str();

    serializeJson(jsonLog, jsonMessage);

    if( _serialOn && _logginglevel == LOGGING_LEVEL_HIGH ) {
        LogPrefix(LOG_INFO, TAG_STATUS);
        Serial.print("Log message (JSON): ");
        Serial.println(jsonMessage); 
    }

    // Start the connection
    _http->begin(*_client, loggingServiceRequestURL);
    _http->addHeader("Content-Type", "content-type:text/plain");

    int httpCode = _http->POST(jsonMessage);

    _http->end();

    if( httpCode == HTTP_CODE_OK ) {
        if( _serialOn && _logginglevel == LOGGING_LEVEL_HIGH ) {
            LogPrefix(LOG_INFO, TAG_STATUS);
            Serial.printf("Logging to servce: SUCCESS %i", httpCode);
            Serial.println();
        }
        return;
    }
    else {
        if( _serialOn && _logginglevel > LOGGING_LEVEL_LOW ) {
            LogPrefix(LOG_WARNING, TAG_STATUS);
            Serial.printf("Logging to servce: ERROR %i", httpCode);
            Serial.println(); 
        }
        return;
    }

}




#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif