/**
 * @file        NetworkManager.cpp
 * @author      Chris Gregg
 * 
 * @brief       Manages Network Services
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


// Project Libraries
#include "NetworkManager.h"
#include "Logger.h"
#include "IOTDevice.h"


////////////////////////////////////////////
//// Network Settings Class

// Public:

// Resets network settings to default
void ICACHE_FLASH_ATTR NetworkSettings::SetDefaults() {
    wifiMode = NET_DEFAULT_WIFIMODE;
    lastStation = 0;
    apSettings.SetDefaults();
    for( int i = 0; i<NET_MAX_STATIONS; i++ ) stationSettings[i].SetDefaults();
    netCheckSettings.setDefaults();
    dnsSettings.setDefaults();
}


////////////////////////////////////////////
//// Network Manager Class

// Public:

// Starts the network services
void ICACHE_FLASH_ATTR NetworkManager::Begin( NetworkSettings& settings ) {

    LOG( PSTR("(Network) Starting network services") );

    _settings = &settings;

    // If double reset start in AP mode
    if( device.GetStartMode() == IOTDevice::DOUBLERESET ) {
        LOG( PSTR("(Network) Double Reset - starting in AP Mode") );
        _settings->wifiMode = WIFI_AP;
    }

    // Start all network services
    _wifi.Begin( _settings->stationSettings, _settings->apSettings, _settings->wifiMode );
    _dns.Begin( _settings->dnsSettings, _wifi.IsAPRunning() );
    _netCheck.Begin( _wifi.GetWiFiClient(), _settings->netCheckSettings );

}


// Get the status of the network
NetworkManager::NetworkStatus ICACHE_FLASH_ATTR NetworkManager::GetNetworkStatus() {
    if( _netCheck.isInternetConnected() ) return NetworkStatus::NORMAL;
    else {
        if( _wifi.IsStationConnected() || _wifi.CountAPConnections() > 0 ) return NetworkStatus::NOINETERNET;
        else return NetworkStatus::DISCONNECTED;
    }
}


// Handle any repeating network tasks
void NetworkManager::Handle() {
    _wifi.Handle();
    _netCheck.Handle();
    _dns.Handle();
}


// Create the global network instance
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETWORK_MANAGER)
    NetworkManager network;
#endif