/**
 * @file        WiFiManager.cpp
 * @author      Chris Gregg
 * 
 * @brief       Manages WiFi Functions
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
#include "WiFiManager.h"
#include "Logger.h"
#include "Env.h"


////////////////////////////////////////////
//// WiFi Station Settings Class

// Public:

// Resets a WiFi station setting to defaults 
void ICACHE_FLASH_ATTR StationSettings::SetDefaults() {
    strcpy_P( SSID, PSTR("") );
    strcpy_P( password, PSTR("") );
    DHCPMode = NET_DEFAULT_DHCPMODE;
    ip = uint32_t(0x00000000);
    subnet = uint32_t(0x00000000);
    gateway = uint32_t(0x00000000);
    dns1 = uint32_t(0x00000000);
    dns2 = uint32_t(0x00000000);     
}


////////////////////////////////////////////
//// WiFi AP Settings Class

// Public:

// Resets the AP settings to defaults 
void ICACHE_FLASH_ATTR APSettings::SetDefaults() {
    strcpy_P( SSID, flag_DEVICE_CODE );
    strcpy_P( password, flag_DEVICE_CODE );
    channel = NET_DEFAULT_AP_CHANNEL;
    ip = uint32_t(NET_DEFAULT_AP_IP);
    subnet = uint32_t(NET_DEFAULT_AP_SUBNET);
    gateway = uint32_t(NET_DEFAULT_AP_GATEWAY);
}


////////////////////////////////////////////
//// WiFi Settings Class

// Public:

// Resets WiFi settings to default
void ICACHE_FLASH_ATTR WiFiSettings::SetDefaults() {
    wifiMode = NET_DEFAULT_WIFIMODE;
    lastStation = 0;
    apSettings.SetDefaults();
    for( int i = 0; i<NET_MAX_STATIONS; i++ ) stationSettings[i].SetDefaults();
}


////////////////////////////////////////////
//// WiFi Manager Class

// Public:

// Initializes the WiFi Manager
void ICACHE_FLASH_ATTR WiFiManager::Begin( WiFiSettings& settings ) {

    LOG( PSTR("(Network) Starting WiFi") );

    // Initialize WiFi
    _settings = &settings;
    _disconnectedStation = 0;

    WiFi.persistent(false);         // TODO - doc why these
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);

    // Restart to start fresh
    WiFi.mode(WiFiMode::WIFI_OFF);

    Handle(true);   // Cause initial WiFi start

}


// Handle WiFi Connectivity
void WiFiManager::Handle( const bool force ) {
    
    if( force ) ResetConnectedStatus();

    switch( _settings->wifiMode ) {
        case WIFI_AP:
            _isAPRunning = HandleWiFiAP( force );
            break;
        case WIFI_AP_STA:
            _isAPRunning = HandleWiFiAP( force );
            HandleWiFiStations( force );
            break;
        case WIFI_STA:
            HandleWiFiStations( force );
            break;
        default:
            _isAPRunning = false;
            break;
    }
}


// Sets the WiFi Mode
void ICACHE_FLASH_ATTR WiFiManager::SetWiFiMode( WiFiMode mode ) {
    _settings->wifiMode = mode;
    Handle( true );
}


// Reconnects all WiFi networks
void ICACHE_FLASH_ATTR WiFiManager::ReconnectWifi() {
    LOG( PSTR("(Network) Reconnecting Wifi") );
    WiFi.setAutoReconnect( false );
    WiFi.disconnect( false );
    Handle( true );                 // Force reconnect
}


// Connects to a specific WiFi station
bool ICACHE_FLASH_ATTR WiFiManager::ConnectWiFiStation( const int id ) {

    LOG( PSTR("(Network) Connecting Wifi Station") );

    ResetConnectedStatus();

    if( _settings->wifiMode != WIFI_STA && _settings->wifiMode != WIFI_AP_STA ) {
        LOG_HIGH( PSTR("Not in station mode") );
        return false;
    }

    LOGF( PSTR("(Network) WiFi mode - Station %i: %s"), id, _settings->stationSettings[id].SSID );

    bool ssid = strcmp( _settings->stationSettings[id].SSID, "" ) != 0;
	bool password = strcmp( _settings->stationSettings[id].password, "" ) != 0;

	if( !ssid ) {
        LOG_HIGH( PSTR("(Network) Station has no SSID") );
        return false;
    }

    bool ret = WiFi.mode( _settings->wifiMode );                     // TODO - do these (and in other places) need to be in the connect function, just the mode change area?

    if( _settings->stationSettings[id].DHCPMode == STATIC ) 
        ret = WiFi.config( _settings->stationSettings[id].ip,
            _settings->stationSettings[id].gateway,
            _settings->stationSettings[id].subnet,
            _settings->stationSettings[id].dns1,
            _settings->stationSettings[id].dns2 );

    WiFi.setAutoReconnect( false );

 //   WiFi.hostname(_settings->dnsSettings.hostname);                   // TODO - why is this commented out
 //   LOGF_HIGH(PSTR("(Network) Network name: %s"),_settings->dnsSettings.hostname);

    if( !password ) ret = WiFi.begin( _settings->stationSettings[id].SSID );
    else ret = WiFi.begin( _settings->stationSettings[id].SSID,
                           _settings->stationSettings[id].password );

	if( ret ) {
		int i = 0;
		while( WiFi.status() != WL_CONNECTED && i++ <= NET_STATION_TRY_TIME ) {
			delay(500);
			if( logger.IsSerialOn() && (logger.GetLogLevel() > LOGGING_LEVEL_NORMAL) ) Serial.print( PSTR(".") );
		}
        if( logger.IsSerialOn() && (logger.GetLogLevel() > LOGGING_LEVEL_NORMAL) ) Serial.print( PSTR("\n") );

		ret = ( WiFi.status() == WL_CONNECTED );

        _stationConnected[id] = ret;

        if( ret ) {
            _settings->lastStation = id;
            _settings->stationSettings[id].ip = WiFi.localIP();
            _settings->stationSettings[id].subnet = WiFi.subnetMask();
            _settings->stationSettings[id].gateway = WiFi.gatewayIP();
            _settings->stationSettings[id].dns1 = WiFi.dnsIP(0);
            _settings->stationSettings[id].dns2 = WiFi.dnsIP(1);

    		if( logger.IsSerialOn() ) {
                IPAddress ip;
                ip = _settings->stationSettings[id].ip; LOGF( PSTR("(Network) WiFi station connected - IP: %s"), ip.toString().c_str() );   
                ip = _settings->stationSettings[id].subnet; LOGF_HIGH( PSTR("(Network) Subnet: %s"), ip.toString().c_str() );         
                ip = _settings->stationSettings[id].gateway; LOGF_HIGH( PSTR("(Network) Gateway: %s"), ip.toString().c_str() );
                ip = _settings->stationSettings[id].dns1; LOGF_HIGH( PSTR("(Network) DNS 1: %s"), ip.toString().c_str() );
                ip = _settings->stationSettings[id].dns2; LOGF_HIGH( PSTR("(Network) DNS 2: %s"), ip.toString().c_str() );
            }
        }
        else LOG( PSTR("(Network) WiFi Station not connected") );

    }

    WiFi.setAutoReconnect( ret );

    return ret;
}


// Returns the currently connected WiFi station
uint ICACHE_FLASH_ATTR WiFiManager::GetConnectedStation() {
    for( int i = 0; i < NET_MAX_STATIONS; i++ ) {
        if( _stationConnected[i] ) return i;
    }
    return 0;
}


// Gets the Assigned IP address
char* ICACHE_FLASH_ATTR WiFiManager::GetAssignedIP() {
    static char ip[16];                                     // Create a static char array to keep
    strcpy( ip, WiFi.localIP().toString().c_str() );
    return ip;
}


// Protected:

// Handles any repeating WiFi station tasks
bool WiFiManager::HandleWiFiStations( const bool force ) {

    bool connected = ( WiFi.status() == WL_CONNECTED );

    if( connected && !force ) {
        _disconnectedStation = 0;
        return true;
    }

    // Check to see if there any saved stations
    bool anyStationsSaved = false;
    for( int i = 0; i < NET_MAX_STATIONS && !anyStationsSaved; i++ )
        if( _settings->stationSettings[i].SSID[0] != '\0' ) anyStationsSaved = true;

    if( !anyStationsSaved ) {
        LOG(PSTR("(Network) No saved WiFi Stations"));
        _settings->wifiMode = WIFI_AP;
        return false;
    }

    // Okay, so not connected
    if( _disconnectedStation == 0 && !force ) {
        _disconnectedStation = millis();
        LOG( PSTR("(Network) WiFi not connected") );
        return false;
    }

    // Are we waiting for SDK to try to retry for a period
    if( _disconnectedStation != 0 && ( millis()-_disconnectedStation < NET_STATION_DISCONNECT_TIME ) && !force ) return false;  

    // Turn on AP if we have waited too long
    if( _disconnectedStation != 0 && ( millis()-_disconnectedStation > NET_STATION_SWITCH_TO_AP_TIME ) && !_isAPRunning ) {
        LOG( PSTR("(Network) - Cannot connect to WiFi. Starting AP") );
        _settings->wifiMode = WIFI_AP;
        return false;
    }

    // Try each of the stored stations, starting with last
    bool success = false;

    for( int i = 0; i < NET_MAX_STATIONS && !success; i++ ) {
        int trystation = (i + _settings->lastStation) % NET_MAX_STATIONS;
        success = ConnectWiFiStation( trystation );
    }

    return success;

}


// Handles any repeating WiFi AP tasks
bool WiFiManager::HandleWiFiAP(const bool force) {

    uint connections = WiFi.softAPgetStationNum();

    if( connections != _nAPConnections ) {
        LOGF( PSTR("(Network) WiFi AP - Clients: %i"), connections );
        _nAPConnections = connections;
    }

    if( _isAPRunning && !force ) return true;

    return StartWiFiAccessPoint();
}


// Start the WiFi Access Point
bool ICACHE_FLASH_ATTR WiFiManager::StartWiFiAccessPoint() {

    LOG( PSTR("(Network) WiFi mode - Access Point") );

    bool ret = WiFi.mode( _settings->wifiMode );

    if( !ret ) return false;

    bool ssid = _settings->apSettings.SSID[0] != '\0';
	bool password = _settings->apSettings.password[0] != '\0';

    WiFi.softAPConfig( _settings->apSettings.ip, _settings->apSettings.gateway, _settings->apSettings.subnet );

	if( !ssid ) return false;
    else if( !password ) ret = WiFi.softAP( _settings->apSettings.SSID, NULL, _settings->apSettings.channel );
    else ret = WiFi.softAP( _settings->apSettings.SSID,
                            _settings->apSettings.password,
                            _settings->apSettings.channel );

    if( ret ) LOGF( PSTR("(Network) Access point started with SSID: %s, IP: %s"), _settings->apSettings.SSID, WiFi.softAPIP().toString().c_str() );
    else LOG( PSTR("(Network) WiFi Access point not started") );

    return ret;
}




