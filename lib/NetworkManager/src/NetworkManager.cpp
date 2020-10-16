/* Network Manager Library

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

Manages Network Functions

*/



#include "NetworkManager.h"
#include "Logger.h"
#include "Device.h"



void ICACHE_FLASH_ATTR NetworkSettings::setDefaults() {

    wifiMode = NET_DEFAULT_WIFIMODE;
    lastStation = 0;
    apSettings.setDefaults();
    for( int i = 0; i<NET_MAX_STATIONS; i++ ) stationSettings[i].setDefaults();
    netCheckSettings.setDefaults();
    dnsSettings.setDefaults();

}



// Network Manager Class


// Public:

void ICACHE_FLASH_ATTR NetworkManager::begin( NetworkSettings &settings ) {

    LOG(PSTR("(Network) Starting network services"));

    _settings = &settings;

    if( device.getStartMode() == IOTDevice::DOUBLERESET ) {
        LOG(PSTR("(Network) Double Reset - starting in AP Mode"));
        _settings->wifiMode = WIFI_AP;
    }

    _wifi.begin( _settings->stationSettings, _settings->apSettings, _settings->wifiMode );
    _dns.begin( _settings->dnsSettings, _wifi.isAPRunning() );
    _netCheck.begin( _wifi.getWiFiClient(), _settings->netCheckSettings );

}


NetworkStatus ICACHE_FLASH_ATTR NetworkManager::getNetworkStatus() {
    if( _netCheck.isInternetConnected() )
        return NetworkStatus::NORMAL;
    else {
        if( _wifi.isStationConnected() || _wifi.countAPConnections() > 0 ) return NetworkStatus::NOINETERNET;
        else return NetworkStatus::DISCONNECTED;
    }
};






void NetworkManager::handle() {

    _wifi.handle();
    _netCheck.handle();
    _dns.handle();

}



// Protected:



// Create the global config instance
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETWORK_MANAGER)
    NetworkManager network;
#endif