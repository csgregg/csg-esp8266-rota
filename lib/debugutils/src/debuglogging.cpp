/* Library Info and License
*/



#include "debuglogging.h"

LogClient::LogClient() {

}



void LogClient::begin( HTTPClient &http, WiFiClient &client ) {
    _http = &http;
    _client = &client;

    Serial.begin(device.monitorBaud);
    Serial.printf("\n\nLOG: %s :: %s - Starting Logging\n", c_log_tag_descript[TAG_STATUS], c_log_type_descript[LOG_INFO]); 
}


void LogClient::setMode( bool modeSerial, bool modeService, t_logging_level level ){
    _serialOn = modeSerial;
    _serviceOn = modeService;
    _logginglevel = level;


    logger.printf("Logging set at level: %i", device.loggingLevel);

    if(_serviceOn) logger.println(LOG_INFO, TAG_STATUS, "Logging Service: ON");
    else logger.println(LOG_INFO, TAG_STATUS, "Logging Service: OFF");
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
            Serial.println("LogClient:: Buffer error");
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }
    len = Serial.write((const uint8_t*) buffer, len);
    if (buffer != temp) {
        delete[] buffer;
    }
    Serial.println();
}

void LogClient::println(t_log_type type, t_log_tag tag, const String &s) {

    if( _logginglevel == LOGGING_OFF ) return;
    if( _logginglevel == LOGGING_LEVEL_LOW && type != LOG_CRITICAL ) return;
    if( _logginglevel == LOGGING_LEVEL_MED && type == LOG_INFO ) return;
    
        if( _serialOn ) {
        Serial.printf("LOG: %s :: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);
        Serial.println(s);
    }

    if( _serviceOn ) LogToService( c_log_tag_descript[tag], s );

}

void LogClient::println(t_log_type type, t_log_tag tag, const char c[]) {

    String message(c);

    if( _logginglevel == LOGGING_OFF ) return;
    if( _logginglevel == LOGGING_LEVEL_LOW && type != LOG_CRITICAL ) return;
    if( _logginglevel == LOGGING_LEVEL_MED && type == LOG_INFO ) return;
    
    if( _serialOn ) {
        Serial.printf("LOG: %s :: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);
        Serial.println(c);
    }

    if( _serviceOn ) LogToService( c_log_tag_descript[tag], message );

}

void LogClient::println(t_log_type type, t_log_tag tag, char c) {

    String message(c);

    if( _logginglevel == LOGGING_OFF ) return;
    if( _logginglevel == LOGGING_LEVEL_LOW && type != LOG_CRITICAL ) return;
    if( _logginglevel == LOGGING_LEVEL_MED && type == LOG_INFO ) return;
    
    if( _serialOn ) {        
        Serial.printf("LOG: %s :: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);
        Serial.println(c);
    }

    if( _serviceOn ) LogToService( c_log_tag_descript[tag], message );

}



size_t LogToSerial(t_log_type type, t_log_tag tag, const String &s){

}


size_t LogToSerial(t_log_type type, t_log_tag tag, const char c[]){

}


size_t LogToSerial(t_log_type type, t_log_tag tag, char c){
    Serial.printf("LOG: %s :: %s - ", c_log_tag_descript[tag], c_log_type_descript[type]);
    Serial.println(c);
}



bool LogClient::LogToService(String customTags, String strMessage){
  
      // Prepare to add to global tags
    if( customTags != "" ) customTags = "," + customTags;

    String loggingServiceRequestURL = "http://" + device.loggingService + "/" + device.loggingServiceKey + "/tag/" + device.loggingGlobalTags + customTags  + "/";

    if( _serialOn && _logginglevel == LOGGING_LEVEL_HIGH ) {
        Serial.printf("LOG: %s :: %s - Connecting to: ", c_log_tag_descript[TAG_STATUS], c_log_type_descript[LOG_INFO]);
        Serial.println(loggingServiceRequestURL);
    }

    // Build JSON

    // Use https://arduinojson.org/v6/assistant/
    const size_t capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 454;
    DynamicJsonDocument jsonLog(capacity);
    String jsonMessage;

    jsonLog["localtime"] = millis();

    if(strMessage.length() > MAX_MESSAGE_LEN ) strMessage = strMessage.substring(0, MAX_MESSAGE_LEN);

    jsonLog["message"] = strMessage;

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
        Serial.printf("LOG: %s :: %s - Log message (JSON): ", c_log_tag_descript[TAG_STATUS], c_log_type_descript[LOG_INFO]);
        Serial.println(jsonMessage); 
    }

    // Start the connection
    _http->begin(*_client, loggingServiceRequestURL);
    _http->addHeader("Content-Type", "content-type:text/plain");

    int httpCode = _http->POST(jsonMessage);

    _http->end();

    if( httpCode == HTTP_CODE_OK ) {
        if( _serialOn && _logginglevel == LOGGING_LEVEL_HIGH ) {
            Serial.printf("LOG: %s :: %s - Logging to servce: SUCCESS %i\n", c_log_tag_descript[TAG_STATUS], c_log_type_descript[LOG_INFO], httpCode); 
        }
        return true;
    }
    else {
        if( _serialOn && _logginglevel > LOGGING_LEVEL_LOW ) {
            Serial.printf("LOG: %s :: %s - Logging to servce: ERROR %i\n", c_log_tag_descript[TAG_STATUS], c_log_type_descript[LOG_WARNING], httpCode); 
        }
        return false;
    }

}




#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif