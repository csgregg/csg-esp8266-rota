/**
 * @file        Logger.h
 * @author      Chris Gregg
 * @brief       Packages debug and logging to the serial port and Loggly.com service for
 *              simple logging or debug of code. Macros are defined to simplifiy common usage. 
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

/* MIT License

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
SOFTWARE. */


#ifndef LOGGER_H

    #define LOGGER_H

    // Global Libraries
    #include <Arduino.h>
    #include <ESP8266WiFi.h>
    #include <Ticker.h>


    // #define NO_LOGGING               // Turn off all debug if necessary

    #ifndef LOGGER_LEVEL
        #define NO_LOGGING
    #endif

    #ifndef NO_LOGGING

        // Macros to simplifiy common usage

        #define DEBUG( text ) logger.println( NORMAL_LOG, DEBUG_TAG, text, __FILE__, __FUNCTION__, __LINE__ )
        #define DEBUGF( ... ) logger.printf( NORMAL_LOG, DEBUG_TAG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__ )
        #define DEBUG_STOP Serial.printf( "\n\n(DEBUG) STOP: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__ ); while( true ) { yield(); };
        #define DEBUG_RAW( text ) Serial.println( text );

        #define LOG( text ) logger.println( NORMAL_LOG, STATUS_TAG, text )
        #define LOG_DETAIL( text ) logger.println( DETAIL_LOG, STATUS_TAG, text )
        #define LOG_HIGH( text ) logger.println( HIGH_LOG, STATUS_TAG, text )
        #define LOG_CRITICAL( text ) logger.println( CRITICAL_LOG, STATUS_TAG, text )

        #define LOGF( ... ) logger.printf( NORMAL_LOG, STATUS_TAG, __VA_ARGS__ )
        #define LOGF_DETAIL( ... ) logger.printf( DETAIL_LOG, STATUS_TAG, __VA_ARGS__ )
        #define LOGF_HIGH( ... ) logger.printf( HIGH_LOG, STATUS_TAG, __VA_ARGS__ )
        #define LOGF_CRITICAL( ... ) logger.printf( CRITICAL_LOG, STATUS_TAG, __VA_ARGS__ )

        #define LOG_FLAG( name ) logger.printFlag( NORMAL_LOG, STATUS_TAG, #name, name )      // Used to pass flag name as argument to method

    #else

        // Empty macros which do nothing when logging fully turned off in build

        #define DEBUG( text )
        #define DEBUGF( ... )
        #define DEBUG_STOP()
        #define DEBUG_RAW( text )

        #define LOG( text )
        #define LOG_DETAIL( text )
        #define LOG_HIGH( text )
        #define LOG_CRITICAL( text )

        #define LOGF( ... )
        #define LOGF_DETAIL( ... )
        #define LOGF_HIGH( ... )
        #define LOGF_CRITICAL( ... )

        #define LOG_FLAG( text )

    #endif


    // Sizes
    #define LOG_MAX_MESSAGE_LEN 256     // Longest message to be processed. Truncated otherwise
    #define LOG_MAX_GLOBAL_TAG_LEN 8    // Max length of global tag string
    #define LOG_MAX_KEY_LEN 40          // Max length of service key string
    #define LOG_MAX_SERVICE_LEN 32      // Max length of service URL
    #define LOG_MAX_LOG_TYPES 4         // Safety to ensure we don't define more than we can handle
    #define LOG_MAX_TAG_TYPES 2         // Safety to ensure we don't define more than we can handle
    #define LOG_MAX_TAG_DESC_LEN 7      // Safety to ensure we don't overflow on size


    // Logging level to filter logs 
    enum LogLevel : uint {
        LOGGING_OFF = 0,                // None
        LOGGING_LEVEL_CRITICAL = 1,     // Critical only
        LOGGING_LEVEL_NORMAL = 2,       // Normal and Critical
        LOGGING_LEVEL_HIGH = 3,         // Normal, Critial and High
        LOGGING_LEVEL_VERBOSE = 4       // All (Detail, Normal, High and Critical)
    };


    // Type of log - used in the filter of log level and added to message
    enum LogType : uint {
        CRITICAL_LOG = 0,
        NORMAL_LOG = 1,
        HIGH_LOG = 2, 
        DETAIL_LOG = 3
    } ;


    // Tags applied to message
    enum LogTag : uint {
        DEBUG_TAG,
        STATUS_TAG
    } ;


    // Type names
    static const char cCritical[] PROGMEM = "CRITICAL";
    static const char cNormal[] PROGMEM = "Normal";
    static const char cHigh[] PROGMEM = "High";
    static const char cVerbose[] PROGMEM = "Verbose";
    #define LOG_MAX_TYPE_DESC_LEN 9

    // Tag names
    static const char cDebug[] PROGMEM = "DEBUG ";
    static const char cStatus[] PROGMEM = "Status";


    /** @class Logger settings
     *  @brief A data structure class that contains the Logger settings. */
    class LoggerSettings
    {

        public:

            /** Resets logger settings to defaults */
            void ICACHE_FLASH_ATTR SetDefaults();

            bool serialModeOn;                          // Serial port on or off
            uint serialBaud;                            // Serial port baud

            bool serviceModeOn;                         // Loggly service on or off           
            char serviceURL[LOG_MAX_SERVICE_LEN];       // URL for Loggly service (doesn't include 'http://')
            char serviceKey[LOG_MAX_KEY_LEN];           // Token for Loggy service
            bool tickModeOn;                            // Loggly tick on or off
            uint tickInterval;                          // Time interval for Loggly tick

            char globalTags[LOG_MAX_GLOBAL_TAG_LEN];    // Tags used on all Loggly posts
            LogLevel level = LOGGING_LEVEL_NORMAL;      // Current logging level

            // Create a compare operators
            
            bool operator== ( const LoggerSettings& other ) const {
                return serialBaud == other.serialBaud
                    && ( strcmp( serviceURL, other.serviceURL ) == 0 )
                    && ( strcmp( serviceKey, other.serviceKey ) == 0 )
                    && serialModeOn == other.serialModeOn
                    && serviceModeOn == other.serviceModeOn
                    && tickModeOn == other.tickModeOn
                    && tickInterval == other.tickInterval
                    && ( strcmp( globalTags, other.globalTags ) == 0 )
                    && level == other.level;
            }
            bool operator!= ( const LoggerSettings& other ) const {
                return serialBaud != other.serialBaud
                    || ( strcmp( serviceURL, other.serviceURL ) != 0 )
                    || ( strcmp( serviceKey, other.serviceKey ) != 0 )
                    || serialModeOn != other.serialModeOn
                    || serviceModeOn != other.serviceModeOn
                    || tickModeOn != other.tickModeOn
                    || tickInterval != other.tickInterval
                    || ( strcmp( globalTags, other.globalTags ) != 0 )
                    || level != other.level;
            }

    };

    
    /** @class LogClient
     *  @brief Sets up the logger and performs logging actions to serial and Loggly service */
    class LogClient
    {
        
        public:

            /** Sets up logging service
             *  @param client    Reference to a WiFiClient to reuse 
             *  @param settings  The settings struct to use */
            void ICACHE_FLASH_ATTR Begin( WiFiClient& client, LoggerSettings& settings );
            
            /** Restarts logging service
             *  @param settings  The settings struct to use */
            void ICACHE_FLASH_ATTR Restart( LoggerSettings& settings );

            /** Gets the status of the serial logging mode
             *  @return true:        Serial logging is on */
            bool ICACHE_FLASH_ATTR IsSerialOn() { return _settings->serialModeOn; }
            
            /** Gets the current logging level
             *  @return  LogLevel */
            LogLevel ICACHE_FLASH_ATTR GetLogLevel() { return _settings->level; }

            // Log messages - println function overloads

            /** Logs a message
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param message   char array to send as message */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const char* message);
            /** Logs a message (with context)
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param message   char array to send as message
             *  @param file      filename of this file - added at compile time 
             *  @param func_P    name of current function where this has been called from - added at compile time
             *  @param line      line in file where this has been called from - added at compile time */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const char* message, const char* file, const char* func_P, const int line );
            /** Logs a message from a single character
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param c         character to send as message */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const char c);
            /** Logs a message from a single character (with context)
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param c         character to send as message
             *  @param file      filename of this file - added at compile time 
             *  @param func_P    name of current function where this has been called from - added at compile time
             *  @param line      line in file where this has been called from - added at compile time */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const char c, const char* file, const char* func_P, const int line );    
            /** Logs a message from an int
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log 
             *  @param i         int to send as message */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const int i);
            /** Logs a message from an int (with context)
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log 
             *  @param i         int to send as message
             *  @param file      filename of this file - added at compile time 
             *  @param func_P    name of current function where this has been called from - added at compile time
             *  @param line      line in file where this has been called from - added at compile time */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const int i, const char* file, const char* func_P, const int line );    
            /** Logs a message from an int
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log 
             *  @param message   String to send as message */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const String& message );
            /** Logs a message from an int (with context)
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log 
             *  @param message   String to send as message
             *  @param file      filename of this file - added at compile time 
             *  @param func_P    name of current function where this has been called from - added at compile time
             *  @param line      line in file where this has been called from - added at compile time */
            void ICACHE_FLASH_ATTR println( const LogType type, const LogTag tag, const String& message, const char* file, const char* func_P, const int line );      

            // Log messages - printf function overloads

            /** Logs a formated message 
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log 
             *  @param format    standard 'printf' formatting
             *  @param ...       arguemnts for formating */
            void ICACHE_FLASH_ATTR printf( const LogType type, const LogTag tag, const char* format, ...);
            /** Logs a formated message 
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param file      filename of this file - added at compile time 
             *  @param func_P    name of current function where this has been called from - added at compile time
             *  @param line      line in file where this has been called from - added at compile time
             *  @param format    standard 'printf' formatting
             *  @param ...       arguemnts for formating */           
            void ICACHE_FLASH_ATTR printf( const LogType type, const LogTag tag, const char* file, const char* func_P, const int line, const char* format, ... );

            //  Log build flags - printFlag function overloads

            /** Logs build flag
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param name      Name of build flag 
             *  @param flag      Build flag as a char array */
            void ICACHE_FLASH_ATTR printFlag( const LogType type, const LogTag tag, const char* name, const char* flag );
            /** Logs build flag
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param name      Name of build flag 
             *  @param flag      Build flag as a bool */
            void ICACHE_FLASH_ATTR printFlag( const LogType type, const LogTag tag, const char* name, const bool flag );
            /** Logs build flag
             *  @param type      'type' of log to log
             *  @param tag       tags to apply to log
             *  @param name      Name of build flag 
             *  @param flag      Build flag as a unsigned int */
            void ICACHE_FLASH_ATTR printFlag( const LogType type, const LogTag tag, const char* name, const uint flag );

            void Handle();      /** Handles any repeating device actions */ 


        protected:

            /** Sends the log type and tag to the serial port
             *  Also adds timer and head if in verbose mode
             *  @param type      'type' of log to log    
             *  @param tag       tags to apply to */
            void inline LogPrefix(const LogType type, const LogTag tag);

            /** Logs a message to the serial port
             *  @param type      'type' of log to log    
             *  @param tag       tags to apply to
             *  @param message   message to send as a char array */
            void LogToSerial( const LogType type, const LogTag tag, const char* message );

            /** Logs a message to the Loggy service
             *  @param type      'type' of log to log    
             *  @param tag       tags to apply to
             *  @param message   message to send asd a char array */
            void LogToService( const LogType type, const LogTag tag, const char* message );

            /** Logs a tick to the Loggy service containing the minimum data set */
            void HandleTick();
            
            /** Function called by Ticker interrupt */
            static void TriggerTick() { _doTick = true; }

            WiFiClient* _client;            // Pointer to reuseable WiFiClient
            LoggerSettings* _settings;      // Pointer to the data struct holding the logger settings

            char _fullServiceURL[LOG_MAX_GLOBAL_TAG_LEN+LOG_MAX_KEY_LEN+LOG_MAX_SERVICE_LEN+16];    // The URL for the Loggly service, fully prepared

            Ticker _tickCheck;              // Ticker object to trigger tick events to Loggly service
            static bool _doTick;            // Flag that indicates time to do send a tick

            // Array of descriptions for the log types
            const char* const _logTypeDescriptions[LOG_MAX_LOG_TYPES] = { cCritical, cNormal, cHigh, cVerbose };
            // Array of descriptions for the log tags
            const char* const _logTagDescriptions[LOG_MAX_TAG_TYPES] = { cDebug, cStatus };

    };


    extern LogClient logger;        // Declaring the global instance

#endif              // LOGGER_H

