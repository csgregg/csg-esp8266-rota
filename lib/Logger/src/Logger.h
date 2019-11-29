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

Use https://arduinojson.org/v6/assistant/ to determine size of file.

*/


#ifndef DEBUGLOGGING_H

    #define DEBUGLOGGING_H

    #include "IOTDevice.h"

    #define MAX_MESSAGE_LEN 256     // Longest message to be processed. Truncated otherwise

    #ifndef NO_DEBUG

      // Macros to simplifiy common usage

      #define DEBUG_DETAIL(text) logger.println(LOG_DETAIL, TAG_DEBUG, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
      #define DEBUG(text) logger.println(LOG_NORMAL, TAG_DEBUG, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
      #define DEBUG_CRITICAL(text) logger.println(LOG_CRITICAL, TAG_DEBUG, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)

      #define LOG_DETAIL(text) logger.println(LOG_DETAIL, TAG_STATUS, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
      #define LOG(text) logger.println(LOG_NORMAL, TAG_STATUS, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)
      #define LOG_CRITICAL(text) logger.println(LOG_CRITICAL, TAG_STATUS, text, __FILE__, FPSTR(__FUNCTION__), __LINE__)

      #define DEBUG_STOP() while(true){yield();}
      #define DEBUG_RAW(text) Serial.println(text)

    #else

      #define DEBUG_DETAIL(text)
      #define DEBUG(text)
      #define DEBUG_CRITICAL(text)

      #define LOG_DETAIL(text)
      #define LOG(text)
      #define LOG_CRITICAL(text)

      #define DEBUG_STOP()
      #define DEBUG_RAW(text)

    #endif

    
    // Logging level to filter logs 

    typedef enum : int {
        LOGGING_OFF = 0,                // None
        LOGGING_LEVEL_CRITICAL = 1,     // Critical only
        LOGGING_LEVEL_NORMAL = 2,       // Normal and critical
        LOGGING_LEVEL_VERBOSE = 3       // All (Detail, Normal, Critical)
    } t_logging_level;


    // Type of log - used in the filter of log level and added to message

    typedef enum : int {
        LOG_CRITICAL,
        LOG_NORMAL,
        LOG_DETAIL
    } t_log_type;


    // Tags applied to message
    
    typedef enum : int {
        TAG_DEBUG,
        TAG_STATUS
    } t_log_tag;

    
    // Logger Class

    class LogClient {
        
        public:
  
            LogClient();

            /// To set up logging

            void begin( HTTPClient &http, WiFiClient &client );
            void setMode( bool modeSerial = false, bool modeService = false, t_logging_level level = LOGGING_OFF );

            // Log messages (with overloads)

            void println(t_log_type type, t_log_tag tag, const String &s);
            void println(t_log_type type, t_log_tag tag, const char c[]);
            void println(t_log_type type, t_log_tag tag, char c);

            void println(t_log_type type, t_log_tag tag, const String &s, const String &file, const String &func, const int line );

            // Special for formated message

            void setTypeTag(t_log_type type, t_log_tag tag);
            void printf(const char *format, ...);

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

    extern LogClient logger;        // Declaring the global instance

#endif

