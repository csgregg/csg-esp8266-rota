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

-----------------------------------------------------------------------------

Defines the physical attributes of the IOT device and the build environment.

Build flags are loaded from platformio.ini

Builds on Espclass

*/


#include "IOTDevice.h"


// Public:

// Return type String build flag or flag with descriton string
ICACHE_FLASH_ATTR String IOTDevice::getBuildFlag( const char * name, const char * flag, bool described ) {
    if( described ) {

        char buffer[ (sizeof("(Build) %s : %s") - 4 ) + strlen_P(flag) ];
        sprintf(buffer, PSTR("(Build) %s : %s"), name, FPSTR(flag));

        return String(buffer);
    }
    else return FPSTR(flag);
}


// Return type bool build flag
ICACHE_FLASH_ATTR bool IOTDevice::IOTDevice::getBuildFlag( const char * name, const bool flag ) {
    return flag;
}


// Return type bool build flag with descripton string
ICACHE_FLASH_ATTR String IOTDevice::getBuildFlag( const char * name, const bool flag, const bool decribed ) {

    char buffer[ (sizeof("(Build) %s : %i") - 4 ) + sizeof(bool)*8 ];
    sprintf(buffer, PSTR("(Build) %s : %i"), name, flag);

    return String(buffer);

}


// Return type uint build flag
ICACHE_FLASH_ATTR uint IOTDevice::IOTDevice::getBuildFlag( const char * name, const uint flag ) {
    return flag;
}


// Return type uint build flag with description string
ICACHE_FLASH_ATTR String IOTDevice::getBuildFlag( const char * name, const uint flag, const bool decribed ) {

    char buffer[ (sizeof("(Build) %s : %i") - 4 ) + sizeof(uint)*8 ];
    sprintf(buffer, PSTR("(Build) %s : %i"), name, flag);

    return String(buffer);

}


// Return type long build flag
ICACHE_FLASH_ATTR long IOTDevice::IOTDevice::getBuildFlag( const char * name, const long flag ) {
    return flag;
}


// Return type long build flag with description string
ICACHE_FLASH_ATTR String IOTDevice::getBuildFlag( const char * name, const long flag, const bool decribed ) {

    char buffer[ (sizeof("(Build) %s : %li") - 5 ) + sizeof(long)*8 ];
    sprintf(buffer, PSTR("(Build) %s : %li"), name, flag);

    return String(buffer);

}


// Get unique chip ID and return as string or with desription string
ICACHE_FLASH_ATTR String IOTDevice::getChipId(bool described){
    
    uint32_t id = EspClass::getChipId();
    char buffer [sizeof("(Device) Chip ID: ") + sizeof(uint32_t)*2];
    sprintf(buffer, PSTR("(Device) Chip ID: %0X10"), id);

    return String(buffer);
}


// Create the global device instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_IOT_DEVICE)
    IOTDevice device;
#endif

