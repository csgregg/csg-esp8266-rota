/* Logger Library

MIT License

Copyright (c) 2020 Chris Gregg

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


#ifndef LOGGER_H

    #define LOGGER_H

    #include <ESP8266WiFi.h>
    #include <Ticker.h>

    // Turn off all debug if necessary
    // #define NO_LOGGING

    #ifndef LOGGER_LEVEL
        #define NO_LOGGING
    #endif

    #ifndef NO_LOGGING

      // Macros to simplifiy common usage

      #define DEBUG(text) logger.println(NORMAL_LOG, DEBUG_TAG, text, __FILE__, __FUNCTION__, __LINE__)
      #define DEBUGF(...) logger.printf(NORMAL_LOG, DEBUG_TAG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
      #define DEBUG_STOP Serial.printf("\n\n(DEBUG) STOP: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__); while(true){yield();};
      #define DEBUG_RAW(text) Serial.println(text);

      #define LOG(text) logger.println(NORMAL_LOG, STATUS_TAG, text)
      #define LOG_DETAIL(text) logger.println(DETAIL_LOG, STATUS_TAG, text)
      #define LOG_HIGH(text) logger.println(HIGH_LOG, STATUS_TAG, text)
      #define LOG_CRITICAL(text) logger.println(CRITICAL_LOG, STATUS_TAG, text)

      #define LOGF(...) logger.printf(NORMAL_LOG, STATUS_TAG, __VA_ARGS__)
      #define LOGF_DETAIL(...) logger.printf(DETAIL_LOG, STATUS_TAG, __VA_ARGS__)
      #define LOGF_HIGH(...) logger.printf(HIGH_LOG, STATUS_TAG, __VA_ARGS__)
      #define LOGF_CRITICAL(...) logger.printf(CRITICAL_LOG, STATUS_TAG, __VA_ARGS__)
  
      #define LOG_FLAG(name) logger.printFlag(NORMAL_LOG, STATUS_TAG, #name, name)      // Used to pass flag name as argument to method

    #else

      #define DEBUG(text)
      #define DEBUGF(...)
      #define DEBUG_STOP()
      #define DEBUG_RAW(text)
      
      #define LOG(text)
      #define LOG_DETAIL(text)
      #define LOG_HIGH(text)
      #define LOG_CRITICAL(text)

      #define LOGF(...)
      #define LOGF_DETAIL(...)
      #define LOGF_HIGH(...)
      #define LOGF_CRITICAL(...)

      #define LOG_FLAG(text)

    #endif

    // TODO - Prefix these defines with 'LOG_'
    // Sizes
    #define MAX_MESSAGE_LEN 256     // Longest message to be processed. Truncated otherwise
    #define MAX_LOG_TYPES 4         // Safety to ensure we don't define more than we can handle
    #define MAX_TAG_TYPES 2
    #define MAX_GLOBAL_TAG_LEN 8    // Max length of global tag string
    #define MAX_KEY_LEN 40          // Max length of service key string
    #define MAX_SERVICE_LEN 32      // Max length of service URL

    // Defaults
    #define DEFAULT_MONITOR_BAUD 115200
    #define DEFAULT_TICK_INTERVAL 60

    // Define max JSON key sizes
    #define JSON_SIZE_LOCALTIME 10
    #define JSON_SIZE_MESSAGE MAX_MESSAGE_LEN
    #define JSON_SIZE_BOARD 20
    #define JSON_SIZE_MAC 17
    #define JSON_SIZE_NAME 30
    #define JSON_SIZE_CODE 20
    #define JSON_SIZE_BUILD 8
    #define JSON_SIZE_HEAP 10
    #define JSON_SIZE_IP 15
    #define JSON_SIZE_SSID 32



    // Logging level to filter logs 
    enum logLevel : uint {
        LOGGING_OFF = 0,                // None
        LOGGING_LEVEL_CRITICAL = 1,     // Critical only
        LOGGING_LEVEL_NORMAL = 2,       // Normal and Critical
        LOGGING_LEVEL_HIGH = 3,         // Normal, Critial and High
        LOGGING_LEVEL_VERBOSE = 4       // All (Detail, Normal, High and Critical)
    };


    // Type of log - used in the filter of log level and added to message
    enum logType : uint {
        CRITICAL_LOG = 0,
        NORMAL_LOG = 1,
        HIGH_LOG = 2, 
        DETAIL_LOG = 3
    } ;


    // Tags applied to message
    enum logTag : uint {
        DEBUG_TAG,
        STATUS_TAG
    } ;

    // Type names
    static const char cCritical[] PROGMEM = "CRITICAL";
    static const char cNormal[] PROGMEM = "Normal";
    static const char cHigh[] PROGMEM = "High";
    static const char cVerbose[] PROGMEM = "Verbose";
    #define MAX_TYPE_DESC_LEN 9

    // Tag names
    static const char cDebug[] PROGMEM = "DEBUG ";
    static const char cStatus[] PROGMEM = "Status";
    #define MAX_TAG_DESC_LEN 7

    // Logger settings
    class LogSettings {

        public:

            void ICACHE_FLASH_ATTR setDefaults();

            uint serialBaud = DEFAULT_MONITOR_BAUD;
            char serviceURL[MAX_SERVICE_LEN] = "";
            char serviceKey[MAX_KEY_LEN] = "";

            bool serialMode = false;
            bool serviceMode = false;

            bool tickMode = false;
            uint tickInterval = DEFAULT_TICK_INTERVAL;

            char globalTags[MAX_GLOBAL_TAG_LEN] = "";
            logLevel level = LOGGING_LEVEL_NORMAL;

             // Create a compare operators

            bool operator==(const LogSettings& other) const {
                return serialBaud == other.serialBaud
                    && (strcmp(serviceURL, other.serviceURL)==0)
                    && (strcmp(serviceKey, other.serviceKey)==0)
                    && serialMode == other.serialMode
                    && serviceMode == other.serviceMode
                    && tickMode == other.tickMode
                    && tickInterval == other.tickInterval
                    && (strcmp(globalTags, other.globalTags)==0)
                    && level == other.level;
            }

            bool operator!=(const LogSettings& other) const {
                return serialBaud != other.serialBaud
                    || (strcmp(serviceURL, other.serviceURL)!=0)
                    || (strcmp(serviceKey, other.serviceKey)!=0)
                    || serialMode != other.serialMode
                    || serviceMode != other.serviceMode
                    || tickMode != other.tickMode
                    || tickInterval != other.tickInterval
                    || (strcmp(globalTags, other.globalTags)!=0)
                    || level != other.level;
            }

    };


    
    // Logger Class
    class LogClient {
        
        public:

            /// To set up logging

            void ICACHE_FLASH_ATTR begin( WiFiClient &client, LogSettings &settings );
            void ICACHE_FLASH_ATTR begin( LogSettings &settings );
            bool ICACHE_FLASH_ATTR SerialOn() { return _settings->serialMode; };
            logLevel ICACHE_FLASH_ATTR LogLevel() { return _settings->level; };
            
 
            // Log messages (with overloads)

            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const char * message);
            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const char * message, const char * file, const char * func_P, const int line );

            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const char c);
            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const char c, const char * file, const char * func_P, const int line );    

            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const int i);
            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const int i, const char * file, const char * func_P, const int line );    

            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const String &message);
            void ICACHE_FLASH_ATTR println(const logType type, const logTag tag, const String &message, const char * file, const char * func_P, const int line );      

            void ICACHE_FLASH_ATTR printf(const logType type, const logTag tag, const char * format, ...);
            void ICACHE_FLASH_ATTR printf(const logType type, const logTag tag, const char * file, const char * func_P, const int line, const char * format, ...);

            // For build flags
            void ICACHE_FLASH_ATTR printFlag(const logType type, const logTag tag, const char* name, const char* flag);
            void ICACHE_FLASH_ATTR printFlag(const logType type, const logTag tag, const char* name, const bool flag);
            void ICACHE_FLASH_ATTR printFlag(const logType type, const logTag tag, const char* name, const uint flag);

            void handle();

        protected:

            WiFiClient* _client;
            LogSettings* _settings;

            char _FullServiceURL[MAX_GLOBAL_TAG_LEN+MAX_KEY_LEN+MAX_SERVICE_LEN+16];

            void inline LogPrefix(const logType type, const logTag tag);

            void LogToSerial(const logType type, const logTag tag, const char * message);
            void LogToService(const logType type, const logTag tag, const char * message);
            bool handleTick();      // Send minimum data to logging service

            Ticker _tickCheck;
            static bool _doTick;
            static void TriggerTick();

            const char* const c_log_type_descript[MAX_LOG_TYPES] = {cCritical,cNormal,cHigh,cVerbose};
            const char* const c_log_tag_descript[MAX_TAG_TYPES] = {cDebug,cStatus};

            LogSettings _preSettings;

        private:

    };

    extern LogClient logger;        // Declaring the global instance

#endif

