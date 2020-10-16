/* WiFi Manager Library

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

WiFi Network Functions

*/




#include "WiFiManager.h"
#include "Logger.h"
#include "Device.h"



// Network Settings 

void ICACHE_FLASH_ATTR StationSettings::setDefaults() {

    strcpy_P( SSID, PSTR("") );
    strcpy_P( password, PSTR("") );
    DHCPMode = NET_DEFAULT_DHCPMODE;
    ip = uint32_t(0x00000000);
    subnet = uint32_t(0x00000000);
    gateway = uint32_t(0x00000000);
    dns1 = uint32_t(0x00000000);
    dns2 = uint32_t(0x00000000);     

}


void ICACHE_FLASH_ATTR APSettings::setDefaults() {

    strcpy_P( SSID, flag_DEVICE_CODE );
    strcpy_P( password, flag_DEVICE_CODE );
    channel = NET_DEFAULT_CHANNEL;
    ip = uint32_t(NET_DEFAULT_STATICIP);
    subnet = uint32_t(NET_DEFAULT_SUBNET);
    gateway = uint32_t(NET_DEFAULT_GATEWAY);

}





// Network Manager Class


// Public:

void ICACHE_FLASH_ATTR WiFiManager::begin( StationSettings* const &stnSettings, APSettings &apSettings, WiFiMode &wifiMode ) {

    LOG(PSTR("(Network) Starting WiFi"));

    // Initize WiFi

    _stationSettings = &stnSettings;
    _apSettings = &apSettings;
    _wifiMode = &wifiMode;
  
    _disconnectedStation = 0;

    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);

    // Restart to start fresh
    WiFi.mode(WiFiMode::WIFI_OFF);

    handle(true);   // Initial WiFi start

}




// Protected:

// Handle WiFi Connectivity
void WiFiManager::handle(const bool force) {

    if( force ) ResetConnectedStatus();

    switch( *_wifiMode ) {
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


bool WiFiManager::handleWiFiAP(const bool force) {

    uint connections = WiFi.softAPgetStationNum();

    if( connections != _APConnections ) {
        LOGF( PSTR("(Network) WiFi AP - Clients: %i"), connections );
        _APConnections = connections;
    }

    if( _APRunning && !force ) return true;

    return startWiFiAccessPoint();
}


void ICACHE_FLASH_ATTR WiFiManager::reconnectWifi() {
    LOG("(Network) Reconnecting Wifi");
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(false);
    handle(true);       // Force reconnect
}


bool WiFiManager::handleWiFiStation(const bool force) {

    bool connected = (WiFi.status() == WL_CONNECTED);

    if( connected && !force ) {
        _disconnectedStation = 0;
        return true;
    }

    bool anystns = false;
    for( int i = 0; i < NET_MAX_STATIONS && !anystns; i++ ) {
        if( _stationSettings[i]->SSID[0] != '\0' ) anystns = true;
    }
    if( !anystns ) {
        LOG(PSTR("(Network) No saved WiFi Stations"));
        *_wifiMode = WIFI_AP;
        return false;
    }

    // Okay, so not connected
    if( _disconnectedStation == 0 && !force ) {
        _disconnectedStation = millis();
        LOG(PSTR("(Network) WiFi not connected"));
        return false;
    }

    // Are we waiting for SDK to try to retry for a period
    if( _disconnectedStation != 0 && (millis()-_disconnectedStation < NET_STATION_DISCONNECT_TIME) && !force ) {
        return false;  
    }

    // Turn on AP if we have waited too long
    if(_disconnectedStation != 0 && (millis()-_disconnectedStation > NET_STATION_SWITCH_TO_AP_TIME) && !_APRunning ) {
        LOG(PSTR("(Network) - Cannot connect to WiFi. Starting AP"));
        *_wifiMode = WIFI_AP;
        return false;
    }

    // Try each of the stored stations, starting with last
    bool success = false;
    
    for( int i = 0; i < NET_MAX_STATIONS && !success; i++ ) {
        int trystation = (i + _lastStation) % NET_MAX_STATIONS;
        success = connectWiFiStation( trystation );
    }

    return success;

}


bool ICACHE_FLASH_ATTR WiFiManager::startWiFiAccessPoint() {
    LOG(PSTR("(Network) WiFi mode - Access Point"));

    bool ret = WiFi.mode( *_wifiMode );

    if( !ret ) return false;

    bool ssid = _apSettings->SSID[0] != '\0';
	bool password = _apSettings->password[0] != '\0';

    WiFi.softAPConfig( _apSettings->ip, _apSettings->gateway, _apSettings->subnet );

	if( !ssid ) return false;
    else if( !password ) ret = WiFi.softAP( _apSettings->SSID, NULL, _apSettings->channel );
    else ret = WiFi.softAP( _apSettings->SSID,
                            _apSettings->password,
                            _apSettings->channel );

    if( ret ) {
        LOGF( PSTR("(Network) Access point started with SSID: %s, IP: %s"), _apSettings->SSID, WiFi.softAPIP().toString().c_str() );
	}
    else LOG(PSTR("(Network) WiFi Access point not started"));

    return ret;
}


bool ICACHE_FLASH_ATTR WiFiManager::connectWiFiStation( const int id ) {
    LOG(PSTR("(Network) Connecting Wifi Station"));

    ResetConnectedStatus();

    if( *_wifiMode != WIFI_STA && *_wifiMode != WIFI_AP_STA ) {
        LOG_HIGH(PSTR("Not in station mode"));
        return false;
    }

    LOGF( PSTR("(Network) WiFi mode - Station %i: %s"), id, _stationSettings[id]->SSID );

    bool ssid = strcmp( _stationSettings[id]->SSID, "") != 0;
	bool password = strcmp( _stationSettings[id]->password, "") != 0;

	if( !ssid ) {
        LOG_HIGH(PSTR("(Network) Station has no SSID"));
        return false;
    }

    bool ret = WiFi.mode( *_wifiMode );

    if( _stationSettings[id]->DHCPMode == STATIC ) 
        ret = WiFi.config( _stationSettings[id]->ip,
            _stationSettings[id]->gateway,
            _stationSettings[id]->subnet,
            _stationSettings[id]->dns1,
            _stationSettings[id]->dns2 );

    WiFi.setAutoReconnect(false);
 //   WiFi.hostname(_settings->dnsSettings.hostname);
 //   LOGF_HIGH(PSTR("(Network) Network name: %s"),_settings->dnsSettings.hostname);

    if( !password ) ret = WiFi.begin( _stationSettings[id]->SSID );
    else ret = WiFi.begin( _stationSettings[id]->SSID,
                           _stationSettings[id]->password );

	if( ret ) {
		int i = 0;
		while( WiFi.status() != WL_CONNECTED && i++ <= NET_STATION_TRY_TIME ) {
			delay(500);
			if( logger.SerialOn() && (logger.LogLevel() > LOGGING_LEVEL_NORMAL) ) Serial.print(PSTR("."));
		}
        if( logger.SerialOn() && (logger.LogLevel() > LOGGING_LEVEL_NORMAL) ) Serial.print(PSTR("\n"));

		ret = (WiFi.status() == WL_CONNECTED);

        _stationConnected[id] = ret;

        if( ret ) {
            _lastStation = id;
            _stationSettings[id]->ip = WiFi.localIP();
            _stationSettings[id]->subnet = WiFi.subnetMask();
            _stationSettings[id]->gateway = WiFi.gatewayIP();
            _stationSettings[id]->dns1 = WiFi.dnsIP(0);
            _stationSettings[id]->dns2 = WiFi.dnsIP(1);

    		if( logger.SerialOn() ) {
                IPAddress ip;
                ip = _stationSettings[id]->ip; LOGF( PSTR("(Network) WiFi station connected - IP: %s"), ip.toString().c_str());   
                ip = _stationSettings[id]->subnet; LOGF_HIGH( PSTR("(Network) Subnet: %s"), ip.toString().c_str());         
                ip = _stationSettings[id]->gateway; LOGF_HIGH( PSTR("(Network) Gateway: %s"), ip.toString().c_str());
                ip = _stationSettings[id]->dns1; LOGF_HIGH( PSTR("(Network) DNS 1: %s"), ip.toString().c_str());
                ip = _stationSettings[id]->dns2; LOGF_HIGH( PSTR("(Network) DNS 2: %s"), ip.toString().c_str());
            }
        }
        else LOG(PSTR("(Network) WiFi Station not connected"));

    }

    WiFi.setAutoReconnect(ret);

    return ret;
}



