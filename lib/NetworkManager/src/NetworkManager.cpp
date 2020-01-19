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
#include <ESP8266WebServer.h>
#include <FS.h> 

#include <EmbAJAX.h>

#include "NetworkManager.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "website.h"
#include "IOTDevice.h"


// Network Settings 

void StationConfig::setDefaults() {

    strcpy( SSID, DEFAULT_WIFI_SSID );
    strcpy( password, DEFAULT_WIFI_PWD );
    DHCPMode = DEFAULT_DHCPMODE;
    ip = uint32_t(0x00000000);
    subnet = uint32_t(0x00000000);
    gateway = uint32_t(0x00000000);
    dns1 = uint32_t(0x00000000);
    dns2 = uint32_t(0x00000000);     

}


void APConfig::setDefaults() {

    strcpy( SSID, device_getBuildFlag(flag_DEVICE_CODE).c_str() );
    strcpy( password, device_getBuildFlag(flag_DEVICE_CODE).c_str() );        // TODO Make secure token for this
    channel = DEFAULT_CHANNEL;
    ip = uint32_t(DEFAULT_STATICIP);
    subnet = uint32_t(DEFAULT_SUBNET);
    gateway = uint32_t(DEFAULT_GATEWAY);

}


void NetworkSettings::setWiFiDefaults() {

    wifiMode = DEFAULT_WIFIMODE;
    lastStation = 0;
    apSettings.setDefaults();
    for( int i = 0; i<MAX_SSIDS; i++ ) stationSettings[i].setDefaults();
}


// TODO - Flash cache


// Network Manager Class

// Public:




void NetworkManager::begin( NetworkSettings &settings ) {

    LOG(F("(Network) Starting network services"));

    _networkSettings = &settings;

    InitializeWiFi();

    website.InitializeWebServer();

}


void NetworkManager::InitializeWiFi() {

    LOG(F("(Network) Starting WiFi"));

    // Initize WiFi
    
    _disconnectedStation = 0;

    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);

    // Restart to start fresh
    WiFi.mode(WiFiMode::WIFI_OFF);

    handleWiFi();   // Initial WiFi start

}


void NetworkManager::handle() {

    handleWiFi();

    website._ajax.loopHook();

}



// Protected:

// Handle WiFi Connectivity
void NetworkManager::handleWiFi() {

    // TODO - Mode handling for disconnects, portal, etc

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

    if( _APRunning ) return true;

    return startWiFiAccessPoint();
}


bool NetworkManager::handleWiFiStation() {

    bool connected = (WiFi.status() == WL_CONNECTED);

    if( connected ) {
        _disconnectedStation = 0;
        return true;
    }

    // Turn on AP if we have waited too long
    if( WiFi.getMode() != WIFI_OFF && _disconnectedStation != 0 && (millis()-_disconnectedStation > STATION_SWITCH_TO_AP_TIME) && !_APRunning ) {
        LOG(F("(Network) - Cannot connect to WiFi. Starting AP"));
        _networkSettings->wifiMode = WIFI_AP;
        return false;
    }

    // Okay, so not connected
    if( WiFi.getMode() != WIFI_OFF && _disconnectedStation == 0 ) {
        _disconnectedStation = millis();
        LOG(F("(Network) WiFi not connected"));
        return false;
    }

    // Are we waiting for SDK to try to retry for a period
    if( WiFi.getMode() != WIFI_OFF && _disconnectedStation != 0 && (millis()-_disconnectedStation < STATION_DISCONNECT_TIME) ) return false;  

    // Try each of the stored stations
    bool success = false;
    for( int station = 0; station < MAX_SSIDS && !success; station++ ) success = startWiFiStation( (station + _networkSettings->lastStation) % MAX_SSIDS );

    return success;

}


bool NetworkManager::startWiFiAccessPoint() {
    
    LOG(F("(Network) WiFi mode - Access Point"));

    bool ret = WiFi.mode( _networkSettings->wifiMode );

    if( !ret ) return false;

    bool ssid = strcmp( _networkSettings->apSettings.SSID, "") != 0;
	bool password = strcmp( _networkSettings->apSettings.password, "") != 0;

    WiFi.softAPConfig( _networkSettings->apSettings.ip, _networkSettings->apSettings.gateway, _networkSettings->apSettings.subnet );

	if( !ssid ) return false;
    else if( !password ) ret = WiFi.softAP( _networkSettings->apSettings.SSID, NULL, _networkSettings->apSettings.channel );
    else ret = WiFi.softAP( _networkSettings->apSettings.SSID,
                            _networkSettings->apSettings.password,
                            _networkSettings->apSettings.channel );

    if( ret ) {
        logger.setTypeTag(LOG_NORMAL,TAG_STATUS);
        logger.printf("(Network) Acess point started with SSID: %s, IP: %s", _networkSettings->apSettings.SSID, WiFi.softAPIP().toString().c_str() );
	}
    else LOG("(Network) WiFi Access point not started");

    return ret;
}


bool NetworkManager::startWiFiStation( const int id ) {

    logger.setTypeTag( LOG_NORMAL, TAG_STATUS );
    logger.printf("(Network) WiFi mode - Station %i: %s", id, _networkSettings->stationSettings[id].SSID );
 
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
		while( WiFi.status() != WL_CONNECTED && i++ <= STATION_TRY_TIME ) {
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