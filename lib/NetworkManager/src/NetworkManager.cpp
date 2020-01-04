/* Network Manager Library

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


*/

#include <ESP8266WiFi.h>


#include "NetworkManager.h"
#include "Logger.h"
#include "ConfigManager.h"


// Network Settings 

void NetworkSettings::setStationDefaults( const int id ) {

    strcpy( stationSettings[id].SSID, DEFAULT_SSID );
    strcpy( stationSettings[id].password, DEFAUL_PWD );
    stationSettings[id].DHCPMode = DEFAULT_DHCPMODE;
    stationSettings[id].ip = uint32_t(0x00000000);
    stationSettings[id].subnet = uint32_t(0x00000000);
    stationSettings[id].gateway = uint32_t(0x00000000);
    stationSettings[id].dns1 = uint32_t(0x00000000);
    stationSettings[id].dns2 = uint32_t(0x00000000);     

}


void NetworkSettings::setAPDefaults() {

    strcpy( apSettings.SSID, device_getBuildFlag(flag_DEVICE_CODE).c_str() );
    strcpy( apSettings.password, device_getBuildFlag(flag_DEVICE_CODE).c_str() );        // TODO Make secure token for this
    apSettings.channel = DEFAULT_CHANNEL;
    apSettings.ip = uint32_t(DEFAULT_STATICIP);
    apSettings.subnet = uint32_t(DEFAULT_SUBNET);
    apSettings.gateway = uint32_t(DEFAULT_GATEWAY);

}


// TODO - Flash cache


// Network Manager Class

// Public:

NetworkManager::NetworkManager() {
    _StationConnected = false;
    _APRunning = false;
    _ConnectedToInternet = false;
}


void NetworkManager::begin( NetworkSettings &settings ) {

    _networkSettings = &settings;
    _disconnectedStation = 0;

    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);

    // Restart to start fresh
    WiFi.mode(WiFiMode::WIFI_OFF);

    LOG(F("(Network) Starting network services"));
    handle();   // Initial WiFi start

}


void NetworkManager::handle() {

    handleWiFi();

}



// Protected:

// Handle WiFi Connectivity
void NetworkManager::handleWiFi() {

    switch( _networkSettings->wifiMode ) {
        case WIFI_AP:
            _APRunning = handleWiFiAP();
            break;
        case WIFI_AP_STA:
            _APRunning = handleWiFiAP();
            _StationConnected = handleWiFiStation();
            break;
        case WIFI_STA:
            _StationConnected = handleWiFiStation();
            break;
        default:
            _StationConnected = false;
            _APRunning = false;
            break;
    }
}


bool NetworkManager::handleWiFiAP() {
    return true;
}


bool NetworkManager::handleWiFiStation() {

    bool connected = (WiFi.status() == WL_CONNECTED);

    if( connected ) {
        _disconnectedStation = 0;
        return true;
    }

    // Okay, so not connected
    if( WiFi.getMode() != WIFI_OFF && _disconnectedStation == 0 ) {
        _disconnectedStation = millis();
        LOG(F("(Network) WiFi disconnected"));
        return false;
    }

    // Are we waiting for SDK to try to retry for a period
    if( WiFi.getMode() != WIFI_OFF && _disconnectedStation != 0 && (millis()-_disconnectedStation < STATION_DISCONNECT_TIME) ) return false;  

    // TODO - Add multiple SSID handling. Just use slot 0 for now
 //   return startWiFiStation( _networkSettings->lastStation );

    bool success = false;
    for( int station = 0; station < MAX_SSIDS && !success; station++ ) {
        success = startWiFiStation( (station + _networkSettings->lastStation) );
    }

    return success;

}


bool NetworkManager::startWiFiAccessPoint() {
    
    LOG(F("(Network) WiFi mode - Access Point"));

    return true;
}


bool NetworkManager::startWiFiStation( const int id ) {

    logger.setTypeTag( LOG_NORMAL, TAG_STATUS );
    logger.printf("(Network) WiFi mode - Station %i", id);
 
    bool ret = WiFi.mode( _networkSettings->wifiMode );
    if( !ret ) return false;
 
    if( _networkSettings->stationSettings[id].DHCPMode == STATIC ) 
        ret = WiFi.config( _networkSettings->stationSettings[id].ip,
            _networkSettings->stationSettings[id].gateway,
            _networkSettings->stationSettings[id].subnet,
            _networkSettings->stationSettings[id].dns1,
            _networkSettings->stationSettings[id].dns2 );

    bool ssid = strcmp( _networkSettings->stationSettings[id].SSID, "") != 0;
	bool password = strcmp( _networkSettings->stationSettings[id].password, "") != 0;

	if( !ssid ) return false;
    else if( !password ) ret = WiFi.begin( _networkSettings->stationSettings[id].SSID );
    else ret = WiFi.begin( _networkSettings->stationSettings[id].SSID,
                           _networkSettings->stationSettings[id].password );

	if( ret ) {
		int i = 0;
		while( WiFi.status() != WL_CONNECTED && i++ <= 20 ) {       // TODO - add #define
			delay(500);
			if( logger.SerialOn() ) Serial.print(".");
		}
        if( logger.SerialOn() ) Serial.print("\n");

		ret = (WiFi.status() == WL_CONNECTED);

        if( ret ) {
            _networkSettings->lastStation = id;
            _networkSettings->stationSettings[id].ip = WiFi.localIP();
            _networkSettings->stationSettings[id].subnet = WiFi.subnetMask();
            _networkSettings->stationSettings[id].gateway = WiFi.gatewayIP();
            _networkSettings->stationSettings[id].dns1 = WiFi.dnsIP(0);
            _networkSettings->stationSettings[id].dns2 = WiFi.dnsIP(1);

    		logger.setTypeTag(LOG_NORMAL, TAG_STATUS);

            if( logger.SerialOn() || logger.SerialOn() ) {
                IPAddress ip;
                
                ip =  _networkSettings->stationSettings[id].ip;
                logger.printf("(Network) WiFi station connected - IP: %s", ip.toString().c_str());
                ip =  _networkSettings->stationSettings[id].subnet;           
                logger.printf("(Network) WiFi station connected - Subnet: %s", ip.toString().c_str());
                ip =  _networkSettings->stationSettings[id].gateway;           
                logger.printf("(Network) WiFi station connected - Gateway: %s", ip.toString().c_str());
                ip =  _networkSettings->stationSettings[id].dns1;
                logger.printf("(Network) WiFi station connected - DNS: %s", ip.toString().c_str());
                ip =  _networkSettings->stationSettings[id].dns2;
                logger.printf("(Network) WiFi station connected - DNS: %s", ip.toString().c_str());
            }
        }
        else LOG("(Network) WiFi Station not connected");
    }

    return ret;
}


bool NetworkManager::checkInternet() {
    return true;
}


// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETWORK_MANAGER)
    NetworkManager network;
#endif