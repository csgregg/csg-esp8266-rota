/* Library Info and License

Assumes serial is not used for anything else

#define DEBUG - turns on debug

Default debug mode -
    Serial - OFF
    Log Service - OFF
    Level - Low (1)

#define DEBUG_SERIAL - turns on serial
#define DEBUG_LOG_SERVICE - turns on log service

#define DEBUG_LEVEL - 1, 2, 3

*/



/* JSON Loggining Format

{
  "localtime": 1234567890,
  "message": "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",
  "Device": {
    "Hardware": {
      "Platform": "12345678901234567890",
      "Board": "12345678901234567890",
      "Framework": "12345678901234567890",
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

  Use https://arduinojson.org/v6/assistant/

*/



#include <stdio.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "iot_device.h"



#ifndef DEBUGLOGGING_H

    #define DEBUGLOGGING_H

    #define MAX_MESSAGE_LEN 140

    typedef enum : int {
        LOGGING_OFF = 0,                // None
        LOGGING_LEVEL_LOW = 1,          // Critical only
        LOGGING_LEVEL_MED = 2,          // Warning and critical
        LOGGING_LEVEL_HIGH = 3          // All (Info, Warning, Critical)
    } t_logging_level;


    typedef enum : int {
        LOG_CRITICAL,
        LOG_WARNING,
        LOG_INFO
    } t_log_type;

    const char* const c_log_type_descript[] = {"CRITICAL","Warning ","Info    "};


    typedef enum : int {
        TAG_DEBUG,
        TAG_STATUS
    } t_log_tag;

    const char* const c_log_tag_descript[] = {"DEBUG ","STATUS"};


    class LogClient {
        public:
  
            LogClient();

            void setMode( bool modeSerial = false, bool modeService = false, t_logging_level level = LOGGING_OFF );

            void begin( HTTPClient &http, WiFiClient &client );

            void printf(const char *format, ...);
            void println(t_log_type type, t_log_tag tag, const String &s);
            void println(t_log_type type, t_log_tag tag, const char c[]);
            void println(t_log_type type, t_log_tag tag, char c);

        protected:

            HTTPClient * _http;
            WiFiClient * _client;

            bool _serialOn = false;
            bool _serviceOn = false;

            t_logging_level _logginglevel = LOGGING_OFF;

            bool LogToService(String customTags, String strMessage);

            size_t LogToSerial(t_log_type type, t_log_tag tag, const String &s);
            size_t LogToSerial(t_log_type type, t_log_tag tag, const char c[]);
            size_t LogToSerial(t_log_type type, t_log_tag tag, char c);

        private:
    };

    extern LogClient logger;

#endif

