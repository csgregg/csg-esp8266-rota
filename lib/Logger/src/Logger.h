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

*/

#ifndef DEBUGLOGGING_H

    #define DEBUGLOGGING_H

    #include <Ticker.h>
    #include <ESP8266httpUpdate.h>

    #include "IOTDevice.h"

    // Turn off all debug if necessary
    #ifndef LOGGER_LEVEL
        #define NO_LOGGING
    #endif

    #define DEFAULT_MONITOR_BAUD 115200

    #ifndef NO_LOGGING

      // Macros to simplifiy common usage

      #define DEBUG_DETAIL(text) logger.println(LOG_DETAIL, TAG_DEBUG, text, __FILE__, __FUNCTION__, __LINE__)
      #define DEBUG(text) logger.println(LOG_NORMAL, TAG_DEBUG, text, __FILE__, __FUNCTION__, __LINE__)
      #define DEBUG_CRITICAL(text) logger.println(LOG_CRITICAL, TAG_DEBUG, text, __FILE__, __FUNCTION__, __LINE__)

      #define LOG_DETAIL(text) logger.println(LOG_DETAIL, TAG_STATUS, text, __FILE__, __FUNCTION__, __LINE__)
      #define LOG(text) logger.println(LOG_NORMAL, TAG_STATUS, text, __FILE__, __FUNCTION__, __LINE__)
      #define LOG_CRITICAL(text) logger.println(LOG_CRITICAL, TAG_STATUS, text, __FILE__, __FUNCTION__, __LINE__)

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


    #define MAX_MESSAGE_LEN 256     // Longest message to be processed. Truncated otherwise

    // Define max JSON key sizes
    #define JSON_SIZE_LOCALTIME 10
    #define JSON_SIZE_MESSAGE MAX_MESSAGE_LEN
    #define JSON_SIZE_PLATFORM 20
    #define JSON_SIZE_BOARD 20
    #define JSON_SIZE_FRAMEWORK 20
    #define JSON_SIZE_MAC 17
    #define JSON_SIZE_NAME 30
    #define JSON_SIZE_CODE 20
    #define JSON_SIZE_BUILD 8
    #define JSON_SIZE_HEAP 10
    #define JSON_SIZE_IP 15
    #define JSON_SIZE_SSID 32

    
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

            /// To set up logging

            void begin( WiFiClient &client, const long baud, const String &service, const String &key, const String &tags );
            void setMode( const bool modeSerial = false, const bool modeService = false, const t_logging_level level = LOGGING_OFF );

            // Log messages (with overloads)

            void println(const t_log_type type, const t_log_tag tag, const char * message);
            void println(const t_log_type type, const t_log_tag tag, const char * message, const char * file, const char * func_P, const int line );

            void println(const t_log_type type, const t_log_tag tag, const char c);
            void println(const t_log_type type, const t_log_tag tag, const char c, const char * file, const char * func_P, const int line );    

            void println(const t_log_type type, const t_log_tag tag, const String &message);
            void println(const t_log_type type, const t_log_tag tag, const String &message, const char * file, const char * func_P, const int line );      

            // TODO Add overload for FlashStrings

            // Special for formated message

            void setTypeTag(const t_log_type type, const t_log_tag tag);
            void printf(const char * format, ...);


        protected:

            WiFiClient * _client;

            String _ServiceURL;

            bool _serialOn = false;
            bool _serviceOn = false;

            t_logging_level _logginglevel = LOGGING_OFF;

            void inline LogPrefix(const t_log_type type, const t_log_tag tag);

            void LogToSerial(const t_log_type type, const t_log_tag tag, const char * message);
            void LogToService(const t_log_type type, const t_log_tag tag, const char * message);

            t_log_type _lasttype;
            t_log_tag _lasttag;

            const char * const c_log_type_descript[3] = {"CRITICAL","Normal","Verbose"};
            const char * const c_log_tag_descript[2] = {"DEBUG","STATUS"};

        private:

    };

    extern LogClient logger;        // Declaring the global instance

#endif

