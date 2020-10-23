/**
 * @file        NetworkSettingsPage.cpp
 * @author      Chris Gregg
 * 
 * @brief       Server-side functions of netsettings.html
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

/* MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software" ), to deal
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
#include "NetworkSetttingsPage.h"
#include "Logger.h"
#include "ConfigManager.h"


////////////////////////////////////////////
//// Network Settings Page Class

// Public:

// Function to initialize AJAX on this page
void ICACHE_FLASH_ATTR NetworkSettingsPage::InitializeAjax(){

    LOG_HIGH( PSTR("(Page) Network Settings - Initialize AJAX") );

    wifi_stn_asip.setValue( network.GetAssignedIP() );

    // AP Settings
    APSettings wifiAP = config.settings.networkSettings.apSettings;
    wifi_ap_ssid.setValue( wifiAP.SSID );
    wifi_ap_pwd.setValue( wifiAP.password );
    wifi_ap_ip.setValue( wifiAP.ip.toString().c_str() );
    wifi_ap_snet.setValue( wifiAP.subnet.toString().c_str() );
    wifi_ap_gtwy.setValue( wifiAP.gateway.toString().c_str() );
    wifi_ap_ch.selectOption( wifiAP.channel );
    wifi_ap_save.setEnabled( false );

    WiFiMode wifimode =  config.settings.networkSettings.wifiMode;
    wifi_mode_stn.setChecked( wifimode == WIFI_STA || wifimode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( wifimode == WIFI_AP || wifimode == WIFI_AP_STA );

    // Connectivity Settings
    char buffer[5];
    NetCheckSettings netStatus = config.settings.networkSettings.netCheckSettings;
    net_ck_enabled.setChecked( netStatus.enabled );
    net_ck_int.setValue( itoa( netStatus.interval, buffer, 10 ) );
    net_ck_url.setValue( netStatus.checkService );
    net_ck_save.setEnabled( false );

    // DNS Settings
    DNSSettings dns = config.settings.networkSettings.dnsSettings;
    dns_enabled.setChecked( dns.dnsEnabled );
    dns_mdns.setChecked( dns.mDnsEnabled );
    dns_name.setValue( dns.hostName );
    dns_save.setEnabled( false );

    // Time and Location Settings
    TimeLocationSettings tlo = config.settings.timelocSettings;
    tlo_ntp.setChecked( tlo.enabled );
    tlo_token.setValue( tlo.ipInfoToken );
    LoadTimeLocation();
    tlo_save.setEnabled( false );

}


// Function to handle AJAX requests for this page
void ICACHE_FLASH_ATTR NetworkSettingsPage::HandleAjax(){

    LOG_HIGH( PSTR("(Page) Network Settings - Handle AJAX") );

    // Save WiFi settings
    if( website.AjaxID == F("wifi_stn_save") ) {
        SaveWifiStation( website.AjaxValue.toInt() );
        return;
    }
    
    // Used to send back basic details of a specific wifi station
    if( website.AjaxID == F("wifi_stn_id") ) {
        wifi_stn_name.setValue( config.settings.networkSettings.stationSettings[wifi_stn_id.GetIntValue()].SSID) ;
        wifi_stn_on.setValue( network.IsStationConnected( wifi_stn_id.GetIntValue() ) );
        return;
    }

    // Load WiFi station into dialog
    if( website.AjaxID == F("wifi_stn_btn") ) {
        LoadWifiStation( wifi_stn_btn.GetIntValue() );
        return;
    }

    // Connect specific WiFi station
    if( website.AjaxID == F("wifi_stn_cnct") ) {
        ConnectWifiStation( wifi_stn_cnct.GetIntValue() );     
        return;
    }

    // Save WiFi mode
    if( website.AjaxID == F("wifi_mode_save") ) {
        SetWifiMode( WiFiMode(website.AjaxValue.toInt()) ); 
        return;
    }

    // Save AP settings
    if( website.AjaxID == F("wifi_ap_save") ) {
        SaveAP();
        return;
    }

    // Save connectivity checker settings
    if( website.AjaxID == F("net_ck_save") ) {
        SaveNetCheck();
        return;
    }

    // Save DNS settings
    if( website.AjaxID == F("dns_save") ) {
        SaveDNS();
        return;
    }

    // Save Time and Location settings
    if( website.AjaxID == F("tlo_save") ) {
        SaveTimeLocation();
        return;
    }

    // Call location detection
    if( website.AjaxID == F("tlo_detect") ) {
        DetectLocation();
        return;
    }

}


// Protected:

// Set the WiFi mode
void ICACHE_FLASH_ATTR NetworkSettingsPage::SetWifiMode(WiFiMode mode) {

    LOG( PSTR("(Page) Network Settings - Set WiFi mode") );
    
    network.SetWiFiMode( mode );
    config.Save();

    wifi_stn_asip.setValue( network.GetAssignedIP() );
    wifi_mode_stn.setChecked( mode == WIFI_STA || mode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( mode == WIFI_AP || mode == WIFI_AP_STA );
    ClearLoader.Call();

}


// Load a specific WiFi station into the dialog
void ICACHE_FLASH_ATTR NetworkSettingsPage::LoadWifiStation( uint id ) {

    LOG_HIGH( PSTR("(Page) Network Settings - Load Wifi Station") );

    wifiStation = config.settings.networkSettings.stationSettings[id];

    wifi_stn_ssid.setValue( wifiStation.SSID );
    wifi_stn_pwd.setValue( wifiStation.password );
    wifi_stn_ip.setValue( wifiStation.ip.toString().c_str() );
    wifi_stn_snet.setValue( wifiStation.subnet.toString().c_str() );
    wifi_stn_gtwy.setValue( wifiStation.gateway.toString().c_str() );
    wifi_stn_dns1.setValue( wifiStation.dns1.toString().c_str() );
    wifi_stn_dns2.setValue( wifiStation.dns2.toString().c_str() );
    wifi_stn_dhcp.setChecked( wifiStation.DHCPMode == DHCP );

}


// Save a specific WiFi station from the dialog
void ICACHE_FLASH_ATTR NetworkSettingsPage::SaveWifiStation( uint id ) {

    LOG_HIGH( PSTR("(Page) Network Settings - Save Wifi Station") );

    strncpy( wifiStation.SSID, wifi_stn_ssid.value(), NET_MAX_SSID_LEN );
    strncpy( wifiStation.password, wifi_stn_pwd.value(), NET_MAX_PASSWORD_LEN );
 
    bool valid = true;
    valid = valid && wifiStation.ip.fromString( wifi_stn_ip.value() );
    valid = valid && wifiStation.subnet.fromString( wifi_stn_snet.value() );
    valid = valid && wifiStation.gateway.fromString( wifi_stn_gtwy.value() );
    valid = valid && wifiStation.dns1.fromString( wifi_stn_dns2.value() );
    valid = valid && wifiStation.dns2.fromString( wifi_stn_dns2.value() );

    wifiStation.DHCPMode = ( wifi_stn_dhcp.isChecked() ? DHCP : STATIC );

    if( valid || wifiStation.DHCPMode == DHCP ) {
        config.settings.networkSettings.stationSettings[id] = wifiStation;
        config.Save();

        if( network.GetConnectedStation() == id ) network.ReconnectWifi();    
    }
    else website.PostMessage( PSTR("Invlaid IP address") );

    // Make the client reload the wifi list
    wifi_stn_asip.setValue( network.GetAssignedIP() );
    LoadWifiList.Call();

}


// Connect a specific WiFi station
void ICACHE_FLASH_ATTR NetworkSettingsPage::ConnectWifiStation( uint id ) {

    LOG_HIGH( PSTR("(Page) Network Settings - Connect Wifi Station") );
    
    if( network.ConnectWiFiStation( id ) ) config.Save();
    else network.ReconnectWifi();

    // Make the client reload the wifi list
    wifi_stn_asip.setValue( network.GetAssignedIP() );
    LoadWifiList.Call();

}


// Save AP settings from page
void ICACHE_FLASH_ATTR NetworkSettingsPage::SaveAP() {

    LOG_HIGH( PSTR("(Page) Network Settings - Save AP")) ;
    
    APSettings ap;
    strncpy( ap.SSID, wifi_ap_ssid.value(), NET_MAX_SSID_LEN );
    strncpy( ap.password, wifi_ap_pwd.value(), NET_MAX_PASSWORD_LEN );
    
    bool valid = true;
    valid = valid && ap.ip.fromString( wifi_ap_ip.value() );
    valid = valid && ap.subnet.fromString( wifi_ap_snet.value() );
    valid = valid && ap.gateway.fromString( wifi_ap_gtwy.value() );
    ap.channel = atoi( wifi_ap_ch.value() );

    if( valid ) {
        config.settings.networkSettings.apSettings = ap;
        config.Save();

        network.ReconnectWifi();
    }
    else  website.PostMessage( PSTR("Invlaid IP address") );

}


// Save connectivity checker settings from page
void ICACHE_FLASH_ATTR NetworkSettingsPage::SaveNetCheck() {

    LOG_HIGH( PSTR("(Page) Network Settings - Save Connection Checker") );

    NetCheckSettings netStatus;
    netStatus.enabled = net_ck_enabled.isChecked();
    strncpy( netStatus.checkService, net_ck_url.value(), NETCHECK_MAX_SERVICE_LEN );
    netStatus.interval = atoi( net_ck_int.value() );

    config.settings.networkSettings.netCheckSettings = netStatus;
    config.Save();

    network.RestartDNS();

}


// Save DNS settings from page
void ICACHE_FLASH_ATTR NetworkSettingsPage::SaveDNS() {

    LOG_HIGH( PSTR("(Page) Network Settings - Save DNS") );

    DNSSettings dns;
    dns.dnsEnabled = dns_enabled.isChecked();
    strncpy( dns.hostName, dns_name.value(), DNS_MAX_HOSTNAME_LEN );
    dns.mDnsEnabled = dns_mdns.isChecked();

    config.settings.networkSettings.dnsSettings = dns;
    config.Save();
    
}


// Save time and location settings from page
void ICACHE_FLASH_ATTR NetworkSettingsPage::SaveTimeLocation() {

    LOG_HIGH( PSTR("(Page) Network Settings - Save Time/Location") );

    strncpy( config.settings.timelocSettings.ipInfoToken, tlo_token.value(), TLO_IPINFO_MAX_TOKEN_LEN );
    config.settings.timelocSettings.enabled = tlo_ntp.isChecked();
    // Note location, if updated, is already saved to config.settings.tloconfig by timelocation.detectlocation

    config.Save();
}


// Load time and location settings to page
void ICACHE_FLASH_ATTR NetworkSettingsPage::LoadTimeLocation() {

    if( timelocation.IsLocationSet() ) {
        tlo_loc.setValue( config.settings.timelocSettings.location.region );
        tlo_tz.setValue( config.settings.timelocSettings.location.timezone );
    }
    else {
        tlo_loc.setValue( "<b>Not set</b>", true );
        tlo_tz.setValue( "<b>Not set</b> <span class=\"w3-small\">(Using UTC)</span>", true );
    }
}


// Call detect location using IPInfo.io
void ICACHE_FLASH_ATTR NetworkSettingsPage::DetectLocation() {
    if( timelocation.DetectLocation() ) LoadTimeLocation();
}


// Create instance of page class and wrap methods for EmbAJAX
NetworkSettingsPage networksettingspage(
    []() { 
        networksettingspage.ajax.handleRequest( 
            []() {
                networksettingspage.HandleAjax();
            }
        ); 
    },
    []() { 
        networksettingspage.InitializeAjax();
    }
);
