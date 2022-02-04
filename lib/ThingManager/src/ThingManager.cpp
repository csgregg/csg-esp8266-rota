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
void ICACHE_FLASH_ATTR ThingManager::Begin(ThingerSettings& settings ){
    Restart( settings );
    digitalWrite(LED_BUILTIN, LEDstatus ? LOW : HIGH);
}


// Handles any repeating device actions
void ICACHE_FLASH_ATTR ThingManager::Handle(){ 
    if( _settings->enabled && network.GetNetworkStatus() == NetworkManager::NetworkStatus::NORMAL ) {
        io->handle();
    }
}

/** Restart Thinger functions */
void ICACHE_FLASH_ATTR ThingManager::Restart( ThingerSettings& settings ){
    _settings = &settings;

    if( _settings->enabled ){

        LOG( PSTR("(Updater) Starting Thinger.io service") );

        if( NULL == io ){
            io = new ThingerClient(_wifiClient, _settings->user, _settings->device, _settings->token);

            io->set_state_listener([&](ThingerClient::THINGER_STATE state){
                switch(state){
                    case ThingerClient::NETWORK_CONNECTING:
                        LOG_HIGH(F("(Thinger) Network connecting"));
                        break;
                    case ThingerClient::NETWORK_CONNECTED:
                        LOG_HIGH(F("(Thinger) Network connected"));
                        break;
                    case ThingerClient::NETWORK_CONNECT_ERROR:
                        LOG_HIGH(F("(Thinger) Network connection error"));
                        break;
                    case ThingerClient::SOCKET_CONNECTING:
                        LOG_HIGH(F("(Thinger) Socket connecting"));
                        break;
                    case ThingerClient::SOCKET_CONNECTED:
                        LOG_HIGH(F("(Thinger) Socket connected"));
                        break;
                    case ThingerClient::SOCKET_CONNECTION_ERROR:
                        LOG_HIGH(F("(Thinger) Socket connection error"));
                        break;
                    case ThingerClient::SOCKET_DISCONNECTED:
                        LOG_HIGH(F("(Thinger) Socket disconnected"));
                        break;
                    case ThingerClient::SOCKET_ERROR:
                        LOG_HIGH(F("(Thinger) Socket error"));
                        break;
                    case ThingerClient::SOCKET_TIMEOUT:
                        LOG_HIGH(F("(Thinger) Socket timeout"));
                        break;
                    case ThingerClient::THINGER_AUTHENTICATING:
                        LOG_HIGH(F("(Thinger) Authenticating"));
                        break;
                    case ThingerClient::THINGER_AUTHENTICATED:
                        LOG_HIGH(F("(Thinger) Authenticated"));
                        break;
                    case ThingerClient::THINGER_AUTH_FAILED:
                        LOG_HIGH(F("(Thinger) Authentication failed"));
                        break;
                    case ThingerClient::THINGER_STOP_REQUEST:
                        LOG_HIGH(F("(Thinger) Stop request"));
                        break;
                }
            });

            
            (*thing.io)["led"] << [](pson& in){
                if(in.is_empty()){
                    in = LEDstatus;
                }
                else{
                    LEDstatus = in;
                    thing.UpdateLED();
                }
            };

        }
    }

}


/** Sends status of LED to Thinger.io */
void ICACHE_FLASH_ATTR ThingManager::SendLEDUpdate( ) {
    io->stream("led");
}


/** Toggle status of LED */
void ICACHE_FLASH_ATTR ThingManager::UpdateLED( ) {
    digitalWrite(LED_BUILTIN, LEDstatus ? LOW : HIGH);
    indexpage.thing_led.setValue(LEDstatus);
    SendLEDUpdate();
    LOGF( PSTR("(Thinger) LED: %s"), LEDstatus ? "On" : "Off" );
}


/** Toggle status of LED */
void ICACHE_FLASH_ATTR ThingManager::ToggleLED( ) {
    LEDstatus = !LEDstatus;
    UpdateLED();
}

ThingManager thing;                             // Create the global instance