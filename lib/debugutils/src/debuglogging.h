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








#ifndef DEBUGLOGGING_H

    #define DEBUGLOGGING_H

    #define MAX_MESSAGE_LEN 140


    #define DEBUG_INFO(text) logger.println(LOG_INFO, TAG_DEBUG, text)
    #define DEBUG_WARN(text) logger.println(LOG_WARNING, TAG_DEBUG, text)
    #define DEBUG_CRIT(text) logger.println(LOG_CRITICAL, TAG_DEBUG, text)


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

            void setTypeTag(t_log_type type, t_log_tag tag);
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

            void inline LogPrefix(t_log_type type, t_log_tag tag);

            void LogToSerial(t_log_type type, t_log_tag tag, String strMessage);
            void LogToService(t_log_type type, t_log_tag tag, String strMessage);

            t_log_type _lasttype;
            t_log_tag _lasttag;

        private:
    };

    extern LogClient logger;

#endif

