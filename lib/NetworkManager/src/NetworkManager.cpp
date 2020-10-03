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


#include <ESP8266HTTPClient.h>

#include "NetworkManager.h"
#include "Logger.h"
#include "Device.h"




// Network Settings 

void ICACHE_FLASH_ATTR StationConfig::setDefaults() {

    strcpy( SSID, "" );
    strcpy( password, "" );
    DHCPMode = DEFAULT_DHCPMODE;
    ip = uint32_t(0x00000000);
    subnet = uint32_t(0x00000000);
    gateway = uint32_t(0x00000000);
    dns1 = uint32_t(0x00000000);
    dns2 = uint32_t(0x00000000);     

}


void ICACHE_FLASH_ATTR APConfig::setDefaults() {

    strcpy_P( SSID, flag_DEVICE_CODE );
    strcpy_P( password, flag_DEVICE_CODE );
    channel = DEFAULT_CHANNEL;
    ip = uint32_t(DEFAULT_STATICIP);
    subnet = uint32_t(DEFAULT_SUBNET);
    gateway = uint32_t(DEFAULT_GATEWAY);

}


void ICACHE_FLASH_ATTR NetCheckConfig::setDefaults() {

    mode = flag_NET_CHECKER;
    strcpy_P( checkService, flag_NET_CHECK_SERVICE );
    interval = flag_NET_CHECK_INTERVAL;

}


void ICACHE_FLASH_ATTR NetworkSettings::setDefaults() {

    wifiMode = DEFAULT_WIFIMODE;
    lastStation = 0;
    apSettings.setDefaults();
    for( int i = 0; i<MAX_SSIDS; i++ ) stationSettings[i].setDefaults();
    netCheckSettings.setDefaults();

}



// Network Manager Class


bool NetworkManager::_doNetCheck = false;

// Public:

void ICACHE_FLASH_ATTR NetworkManager::begin( NetworkSettings &settings ) {

    LOG(PSTR("(Network) Starting network services"));

    _settings = &settings;

    if( device.getStartMode() == IOTDevice::DOUBLERESET ) _settings->wifiMode = WIFI_AP;

    InitializeWiFi();
    InitializeNetCheck();

}


NetworkStatus ICACHE_FLASH_ATTR NetworkManager::getNetworkStatus() {
    if( _ConnectedToInternet ) return NetworkStatus::NORMAL;
    else {
        if( isStationConnected() || _APConnections > 0 ) return NetworkStatus::NOINETERNET;
        else return NetworkStatus::DISCONNECTED;
    }
};


void ICACHE_FLASH_ATTR NetworkManager::InitializeNetCheck() {

    _doNetCheck = _settings->netCheckSettings.mode;
    _ConnectedToInternet = false;

    if( _netCheck.active() ) _netCheck.detach();

    if( _settings->netCheckSettings.mode ) _netCheck.attach( _settings->netCheckSettings.interval, TriggerNetCheck ); 

};


void NetworkManager::TriggerNetCheck() {
    _doNetCheck = true;
}


void NetworkManager::HandleNetCheck() {

    if ( _doNetCheck ) {

        _doNetCheck = false;
        _ConnectedToInternet = false;

        if( WiFi.status() == WL_CONNECTED ) {

            HTTPClient http;

            http.setReuse(false);
            http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

            LOG( PSTR("(Network) Checking for internet") );

            char url[MAX_CHECK_SERVICE_LEN+sizeof("http://")];
            strcpy_P(url,PSTR("http://"));
            strcat(url, _settings->netCheckSettings.checkService);

            http.begin( _client, url );

            http.setUserAgent(PSTR("ESP8266-http-Update"));                            // TODO - Change all instances to literals
            http.addHeader(PSTR("Content-Type"), PSTR("content-type:text/plain"));

            int httpresponse = http.GET();

            http.end();

            _ConnectedToInternet = ( httpresponse == HTTP_CODE_NO_CONTENT );
        }

        LOG_HIGH( _ConnectedToInternet ? PSTR("(Network) Connected") : PSTR("(Network) Not connected") );

    }
}


void ICACHE_FLASH_ATTR NetworkManager::InitializeWiFi() {

    LOG(PSTR("(Network) Starting WiFi"));

    // Initize WiFi
    
    _disconnectedStation = 0;

    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);

    // Restart to start fresh
    WiFi.mode(WiFiMode::WIFI_OFF);

    handleWiFi(true);   // Initial WiFi start

}


void NetworkManager::handle() {

    handleWiFi();
    HandleNetCheck();

}



// Protected:

// Handle WiFi Connectivity
void NetworkManager::handleWiFi(const bool force) {

    // TODO - Mode handling for disconnects, portal, etc

    if( force ) ResetConnectedStatus();

    switch( _settings->wifiMode ) {
        case WIFI_AP:
            _APRunning = handleWiFiAP(force);
            break;
        case WIFI_AP_STA:
            _APRunning = handleWiFiAP(force);
            handleWiFiStation(force);
            break;
        case WIFI_STA:
            handleWiFiStation(force);
            break;
        default:
            _APRunning = false;
            break;
    }
}


bool NetworkManager::handleWiFiAP(const bool force) {

    uint connections = WiFi.softAPgetStationNum();

    if( connections != _APConnections ) {
        LOGF( PSTR("(Network) WiFi AP - Clients: %i"), connections );
        _APConnections = connections;
    }

    // TODO - check use of 'force'
    if( _APRunning && !force ) return true;

    return startWiFiAccessPoint();
}


void ICACHE_FLASH_ATTR NetworkManager::reconnectWifi() {
    LOG("(Network) Reconnecting Wifi");
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(false);
    handleWiFi(true);       // Force reconnect
}


bool NetworkManager::handleWiFiStation(const bool force) {

    bool connected = (WiFi.status() == WL_CONNECTED);

    if( connected && !force ) {
        _disconnectedStation = 0;
        return true;
    }

    bool anystns = false;
    for( int i = 0; i < MAX_SSIDS && !anystns; i++ ) {
        if( strcmp( _settings->stationSettings[i].SSID, "" ) != 0 ) anystns = true;
    }
    if( !anystns ) {
        LOG(PSTR("(Network) No saved WiFi Stations"));
        _settings->wifiMode = WIFI_AP;
        return false;
    }

    // Okay, so not connected
    if( _disconnectedStation == 0 && !force ) {
        _disconnectedStation = millis();
        LOG(PSTR("(Network) WiFi not connected"));
        return false;
    }

    // Are we waiting for SDK to try to retry for a period
    if( _disconnectedStation != 0 && (millis()-_disconnectedStation < STATION_DISCONNECT_TIME) && !force ) {
        return false;  
    }

    // Turn on AP if we have waited too long
    if(_disconnectedStation != 0 && (millis()-_disconnectedStation > STATION_SWITCH_TO_AP_TIME) && !_APRunning ) {
        LOG(PSTR("(Network) - Cannot connect to WiFi. Starting AP"));
        _settings->wifiMode = WIFI_AP;
        return false;
    }

    // Try each of the stored stations, starting with last
    bool success = false;
    
    for( int i = 0; i < MAX_SSIDS && !success; i++ ) {
        int trystation = (i + _settings->lastStation) % MAX_SSIDS;
        success = connectWiFiStation( trystation );
    }

    return success;

}


bool ICACHE_FLASH_ATTR NetworkManager::startWiFiAccessPoint() {
    LOG(PSTR("(Network) WiFi mode - Access Point"));

    bool ret = WiFi.mode( _settings->wifiMode );

    if( !ret ) return false;

    bool ssid = strcmp( _settings->apSettings.SSID, "") != 0;
	bool password = strcmp( _settings->apSettings.password, "") != 0;

    WiFi.softAPConfig( _settings->apSettings.ip, _settings->apSettings.gateway, _settings->apSettings.subnet );

	if( !ssid ) return false;
    else if( !password ) ret = WiFi.softAP( _settings->apSettings.SSID, NULL, _settings->apSettings.channel );
    else ret = WiFi.softAP( _settings->apSettings.SSID,
                            _settings->apSettings.password,
                            _settings->apSettings.channel );

    if( ret ) {
        LOGF( PSTR("(Network) Acess point started with SSID: %s, IP: %s"), _settings->apSettings.SSID, WiFi.softAPIP().toString().c_str() );
	}
    else LOG(PSTR("(Network) WiFi Access point not started"));

    return ret;
}


bool ICACHE_FLASH_ATTR NetworkManager::connectWiFiStation( const int id ) {
    LOG(PSTR("(Network) Connecting Wifi Station"));

    ResetConnectedStatus();

    if( _settings->wifiMode != WIFI_STA &&_settings->wifiMode != WIFI_AP_STA ) {
        LOG(PSTR("Not in station mode"));
        return false;
    }

    LOGF( PSTR("(Network) WiFi mode - Station %i: %s"), id, _settings->stationSettings[id].SSID );

    bool ssid = strcmp( _settings->stationSettings[id].SSID, "") != 0;
	bool password = strcmp( _settings->stationSettings[id].password, "") != 0;

	if( !ssid ) {
        LOG(PSTR("(Network) Station has no SSID"));
        return false;
    }

    bool ret = WiFi.mode( _settings->wifiMode );

    if( _settings->stationSettings[id].DHCPMode == STATIC ) 
        ret = WiFi.config( _settings->stationSettings[id].ip,
            _settings->stationSettings[id].gateway,
            _settings->stationSettings[id].subnet,
            _settings->stationSettings[id].dns1,
            _settings->stationSettings[id].dns2 );

    WiFi.setAutoReconnect(false);

    if( !password ) ret = WiFi.begin( _settings->stationSettings[id].SSID );
    else ret = WiFi.begin( _settings->stationSettings[id].SSID,
                           _settings->stationSettings[id].password );

	if( ret ) {
		int i = 0;
		while( WiFi.status() != WL_CONNECTED && i++ <= STATION_TRY_TIME ) {
			delay(500);
			if( logger.SerialOn() ) Serial.print(".");
		}
        if( logger.SerialOn() ) Serial.print("\n");

		ret = (WiFi.status() == WL_CONNECTED);

        _stationConnected[id] = ret;

        if( ret ) {
            _settings->lastStation = id;
            _settings->stationSettings[id].ip = WiFi.localIP();
            _settings->stationSettings[id].subnet = WiFi.subnetMask();
            _settings->stationSettings[id].gateway = WiFi.gatewayIP();
            _settings->stationSettings[id].dns1 = WiFi.dnsIP(0);
            _settings->stationSettings[id].dns2 = WiFi.dnsIP(1);

    		if( logger.SerialOn() ) {
                IPAddress ip;
                ip = _settings->stationSettings[id].ip;
                LOGF( PSTR("(Network) WiFi station connected - IP: %s"), ip.toString().c_str());
                ip = _settings->stationSettings[id].subnet;        
                LOGF_HIGH( PSTR("(Network) WiFi station connected - Subnet: %s"), ip.toString().c_str());         
                ip = _settings->stationSettings[id].gateway;
                LOGF_HIGH( PSTR("(Network) WiFi station connected - Gateway: %s"), ip.toString().c_str());
                ip = _settings->stationSettings[id].dns1;
                LOGF_HIGH( PSTR("(Network) WiFi station connected - DNS: %s"), ip.toString().c_str());
                ip = _settings->stationSettings[id].dns2;
                LOGF_HIGH( PSTR("(Network) WiFi station connected - DNS: %s"), ip.toString().c_str());
            }
        }
        else LOG(PSTR("(Network) WiFi Station not connected"));

    }

    WiFi.setAutoReconnect(ret);

    return ret;
}




// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETWORK_MANAGER)
    NetworkManager network;
#endif