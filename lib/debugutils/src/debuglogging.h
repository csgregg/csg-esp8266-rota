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

    #define DEBUG_DETAIL(text) logger.println(LOG_DETAIL, TAG_DEBUG, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
    #define DEBUG(text) logger.println(LOG_NORMAL, TAG_DEBUG, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
    #define DEBUG_CRITICAL(text) logger.println(LOG_CRITICAL, TAG_DEBUG, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)

    #define LOG_DETAIL(text) logger.println(LOG_DETAIL, TAG_STATUS, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
    #define LOG(text) logger.println(LOG_NORMAL, TAG_STATUS, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
    #define LOG_CRITICAL(text) logger.println(LOG_CRITICAL, TAG_STATUS, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)


    #define DEBUG_STOP() while(true){yield();}
    #define DEBUG_RAW(text) Serial.println(text)

    typedef enum : int {
        LOGGING_OFF = 0,                // None
        LOGGING_LEVEL_CRITICAL = 1,      // Critical only
        LOGGING_LEVEL_NORMAL = 2,       // Normal and critical
        LOGGING_LEVEL_VERBOSE = 3       // All (Detail, Normal, Critical)
    } t_logging_level;


    typedef enum : int {
        LOG_CRITICAL,
        LOG_NORMAL,
        LOG_DETAIL
    } t_log_type;

    
    typedef enum : int {
        TAG_DEBUG,
        TAG_STATUS
    } t_log_tag;

    
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


            void println(t_log_type type, t_log_tag tag, const String &s, const String &file, const String &func, const int line );


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

            const char* const c_log_type_descript[3] = {"CRITICAL","Normal","Verbose"};
            const char* const c_log_tag_descript[2] = {"DEBUG","STATUS"};

        private:
    };

    extern LogClient logger;

#endif

