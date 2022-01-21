/**
 * @file        Env.cpp
 * @author      Chris Gregg
 * 
 * @brief       Defines the physical attributes of the IOT device and the build environment.
 *              Build flags are loaded from platformio.ini
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


// Global Libraries
#include <DoubleResetDetector.h>

// Project Libraries
#include "Env.h"

// Global Objects
DoubleResetDetector drd( DRD_TIMEOUT, DRD_ADDRESS );


////////////////////////////////////////////
//// Device Manager Class

// Public:

// Sets up the device hardware and build environment
void ICACHE_FLASH_ATTR Env::Begin() {

    // Need to do this because these preprocessor defines seem to get defined at differnet time to the rest
    sprintf_P( _buildNo, PSTR("%i"), flag_BUILD_NO );
    strcpy_P( _buildTime, flag_BUILD_TIMESTAMP );
    sprintf_P( _chipID, PSTR("%0X") ,EspClass::getChipId() );
    strcpy_P( _buildEnv, flag_BUILD_ENV );

    if( drd.detectDoubleReset() ) _startMode = DOUBLERESET;
    
    // Physical IO Setup

    pinMode(LED_BUILTIN, OUTPUT);
}


// Handles any repeating tasks
void ICACHE_FLASH_ATTR Env::Handle(){
  drd.loop();
}


Env device;           // Create the global instance
