/**
 * @file        ThingManager.cpp
 * @author      Chris Gregg
 * 
 * @brief       Defines and manages the connection to Thinger.io
 * 
 * @copyright   Copyright (c) 2022
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


// Build flags for Thinger.io client
#define _DISABLE_TLS_

// Global Libraries
#include <ThingerClient.h>

// Project Libraries
#include "ThingManager.h"
#include "Env.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "NetworkManager.h"
#include "OTAUpdater.h"
#include "TimeLocation.h"
#include "IndexPage.h"


bool LEDstatus = false;

////////////////////////////////////////////
//// Thinger.io Settings Class

// Public:

// Resest the Thinger.io settings to defaults
void ICACHE_FLASH_ATTR ThingerSettings::SetDefaults() {
    enabled = THINGER_DEFAULT_MODE;
    strcpy_P( user, flag_THINGER_USER );
    strcpy_P( device, flag_THINGER_DEVICE );
    strcpy_P( token, flag_THINGER_TOKEN );
}



////////////////////////////////////////////
//// Thinger.io Manager Class

// Public:


// Initializes the Thinger.io service
void ICACHE_FLASH_ATTR ThingManager::Begin(Client& client, ThingerSettings& settings ){
    _client = &client;
    Restart( settings );
    digitalWrite(LED_BUILTIN, LEDstatus ? LOW : HIGH);
}


// Handles any repeating device actions
void ICACHE_FLASH_ATTR ThingManager::Handle(){ 
    if( _settings->enabled && network.GetNetworkStatus() == NetworkManager::NetworkStatus::NORMAL ) io->handle();
}

/** Restart Thinger functions */
void ICACHE_FLASH_ATTR ThingManager::Restart( ThingerSettings& settings ){
    _settings = &settings;

    if( _settings->enabled ){

        LOG( PSTR("(Updater) Starting Thinger.io service") );

        if( NULL == io ){
            io = new ThingerClient(*_client, _settings->user, _settings->device, _settings->token);
            
            (*thing.io)["led"] << [](pson& in){
                if(in.is_empty()){
                    in = LEDstatus;
                }
                else{
                    LEDstatus = in;
                    digitalWrite(LED_BUILTIN, LEDstatus ? LOW : HIGH);
                    indexpage.thing_led.setValue(LEDstatus);
                }
            };

        }
    }

}

ThingManager thing;                             // Create the global instance