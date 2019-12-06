/* IOT Device Library

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

*/


#include "IOTDevice.h"


String IOTDevice::getChipId(bool described){
    
    uint32_t id = EspClass::getChipId();
    char buffer [sizeof("(Device) Chip ID: ") + sizeof(uint32_t)*2];
    sprintf(buffer, PSTR("(Device) Chip ID: %0X10"), id);

    return String(buffer);
}

String IOTDevice::getPlatform( bool described ) {
    if( described ) return String(F("(Device) Platform: ")) + FPSTR(flag_device_platform);
    else return FPSTR(flag_device_platform);
}

String IOTDevice::getBoard( bool described ) {
    if( described ) return String(F("(Device) Framework: ")) + FPSTR(flag_device_board);
    else return FPSTR(flag_device_board);
}

String IOTDevice::getFramework( bool described ) {
    if( described ) return String(F("(Device) Framework: ")) + FPSTR(flag_device_framework);
    else return FPSTR(flag_device_framework);
}

String IOTDevice::getBuild( bool described ) {
    if( described ) return String(F("(Device) Build: ")) + FPSTR(flag_build_tag);
    else return FPSTR(flag_build_tag);
}

String IOTDevice::getEnvironment( bool described ) {
    if( described ) return String(F("(Device) Environment: ")) + FPSTR(flag_build_env);
    else return FPSTR(flag_build_env);
}

String IOTDevice::getCode( bool described ) {
    if( described ) return String(F("(Device) Environment: ")) + FPSTR(flag_device_code);
    else return FPSTR(flag_device_code);
}

String IOTDevice::getName( bool described ) {
    if( described ) return String(F("(Device) Name: ")) + FPSTR(flag_device_name);
    else return FPSTR(flag_device_name);
}

String IOTDevice::getUpdaterRepo( bool described ) {
    if( described ) return String(F("(Updater) Repo: ")) + FPSTR(flag_updater_repo);
    else return FPSTR(flag_updater_repo);
}

String IOTDevice::getUpdaterUser( bool described ) {
    if( described ) return String(F("(Updater) Repo: ")) + FPSTR(flag_updater_user);
    else return FPSTR(flag_updater_user);
}

String IOTDevice::getUpdaterToken( bool described ) {
    if( described ) return String(F("(Updater) Repo: ")) + FPSTR(flag_updater_token);
    else return FPSTR(flag_updater_token);
}

String IOTDevice::getUpdaterService( bool described ) {
    if( described ) return String(F("(Updater) Repo: ")) + FPSTR(flag_updater_service);
    else return FPSTR(flag_updater_service);
}

String IOTDevice::getUpdaterSkip( bool described ) {

    char buffer [sizeof(bool)*8+1];
    itoa(flag_updater_skip, buffer, 10);

    return String(F("(Updater) Update Skip: ")) + buffer;
}

bool IOTDevice::IOTDevice::getUpdaterSkip() {
    return flag_updater_skip;
}

String IOTDevice::getUpdaterInterval( bool described ) {

    char buffer [sizeof(long)*8+1];
    itoa(flag_updater_interval, buffer, 10);
    
    return String(F("(Updater) Update Interval: ")) + buffer;
}

long IOTDevice::getUpdaterInterval() {
    return flag_updater_interval;
}

String IOTDevice::getLoggerAsSerial( bool described ) {
            
    char buffer [sizeof(bool)*8+1];
    itoa(flag_log_as_serial, buffer, 10);

    return String(F("(Logger) Log as Serial: ")) + buffer;
}

bool IOTDevice::getLoggerAsSerial() {
    return flag_log_as_serial;
}

String IOTDevice::getLoggerAsService( bool described ) {
        
    char buffer [sizeof(bool)*8+1];
    itoa(flag_log_as_service, buffer, 10);

    return String(F("(Logger) Log as Service: ")) + buffer;
}

bool IOTDevice::getLoggerAsService() {
    return flag_log_as_service;
}

String IOTDevice::getLoggerLogLevel( bool described ) {
    
    char buffer [sizeof(uint)*8+1];
    itoa(flag_log_level, buffer, 10);

    return String(F("(Logger) Log Level: ")) + buffer;
}

uint IOTDevice::getLoggerLogLevel() {
    return flag_log_level;
}

String IOTDevice::getLoggerService( bool described ) {
    if( described ) return String(F("(Logger) Service: ")) + FPSTR(flag_logging_service);
    else return FPSTR(flag_logging_service);
}

String IOTDevice::getLoggerServiceKey( bool described ) {
    if( described ) return String(F("(Logger) Service Key: ")) + FPSTR(flag_logging_service_key);
    else return FPSTR(flag_logging_service_key);
}

String IOTDevice::getLoggerGlobalTags( bool described ) {
    if( described ) return String(F("(Logger) Global Tags: ")) + FPSTR(flag_logging_global_tags);
    else return FPSTR(flag_logging_global_tags);
}

String IOTDevice::getLoggerBaud( bool described ) {

    char buffer [sizeof(long)*8+1];
    ltoa(flag_monitor_baud, buffer, 10);

    return String(F("(Logger) Monitor Baud: ")) + buffer;
}

long IOTDevice::getLoggerBaud() {
    return flag_monitor_baud;
}






// Create the global device instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_IOT_DEVICE)
    IOTDevice device;
#endif

