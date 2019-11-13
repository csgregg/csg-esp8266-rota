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





#ifndef DEBUGLOGGING_H

    #define DEBUGLOGGING_H

    class LogClient
    {
        public:
            size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));

            size_t print(const String &s);
            size_t print(const char[]);
            size_t print(char);      


            size_t println(const String &s);
            size_t println(const char[]);
            size_t println(char);
    };

    extern LogClient logger;

#endif

