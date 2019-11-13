/* Library Info and License
*/

#include <stdio.h>
#include <HardwareSerial.h>

#include "debuglogging.h"


size_t LogClient::printf(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    char temp[64];
    char* buffer = temp;
    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            return 0;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }
    len = Serial.print(buffer);
    if (buffer != temp) {
        delete[] buffer;
    }
    return len;
}


size_t LogClient::print(const String &s) {
    return Serial.print(s);
}


size_t LogClient::print(const char str[]) {
    return Serial.print(str);
}


size_t LogClient::print(char c) {
    return Serial.print(c);
}


size_t LogClient::println(const String &s) {
    return Serial.println(s);
}

size_t LogClient::println(const char c[]) {
    return Serial.println(c);
}

size_t LogClient::println(char c) {
    return Serial.println(c);
}



#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
    LogClient logger;
#endif