/* Device Library

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

Defines the physical attributes of the IOT device and the build environment.

Build flags are loaded from platformio.ini

*/

#include "Device.h"


// Need to do this because these flags seem to get defined at differnet time to the rest
void ICACHE_FLASH_ATTR IOTDevice::begin() {

    // Need to do this because these flags seem to get defined at differnet time to the rest
    sprintf_P(_build_no,PSTR("%i"),flag_BUILD_NO);
    strcpy_P(_build_time, flag_BUILD_TIMESTAMP);
    sprintf_P(_chipID,PSTR("%0X"),EspClass::getChipId());
    strcpy_P(_buildEnv,flag_BUILD_ENV);

    if( _drd.detectDoubleReset() ) _startMode = DOUBLERESET;
    
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_DEVICE)
    IOTDevice device;
#endif