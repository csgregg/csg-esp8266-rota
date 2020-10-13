/* Website Manager Library

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

Server-side functions of netsettings.html

*/


#include "NetworkSetttingsPage.h"
#include "Logger.h"
#include "ConfigManager.h"



void ICACHE_FLASH_ATTR NetworkSettingsPage::initializeAjax(){

    LOG_HIGH(PSTR("(Page) Network Settings - Initialize AJAX"));

    wifi_stn_asip.setValue(network.getAssignedIP());

    // AP Settings
    APConfig wifiAP = config.settings.networkSettings.apSettings;
    wifi_ap_ssid.setValue(wifiAP.SSID);
    wifi_ap_pwd.setValue(wifiAP.password);
    wifi_ap_ip.setValue(wifiAP.ip.toString().c_str());
    wifi_ap_snet.setValue(wifiAP.subnet.toString().c_str());
    wifi_ap_gtwy.setValue(wifiAP.gateway.toString().c_str());
    wifi_ap_ch.selectOption(wifiAP.channel);
    wifi_ap_save.setEnabled(false);

    WiFiMode wifimode =  config.settings.networkSettings.wifiMode;
    wifi_mode_stn.setChecked( wifimode == WIFI_STA || wifimode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( wifimode == WIFI_AP || wifimode == WIFI_AP_STA );

    // Connectivity Settings
    char buffer[5];
    NetCheckConfig netStatus = config.settings.networkSettings.netCheckSettings;
    net_ck_mode.setChecked( netStatus.mode );
    net_ck_int.setValue( itoa(netStatus.interval,buffer,10) );
    net_ck_url.setValue( netStatus.checkService );
    net_ck_save.setEnabled(false);

    // DNS Settings
    DNSConfig dns = config.settings.networkSettings.dnsSettings;
    dns_mode.setChecked( dns.mode );
    dns_mdns.setChecked( dns.mDNS );
    dns_name.setValue( dns.hostname );
    dns_save.setEnabled(false);

    // Time and Location Settings
    TimeLocationSettings tlo = config.settings.timelocsettings;
    tlo_ntp.setChecked( tlo.ntpMode );
    tlo_token.setValue( tlo.ipinfoToken );
    loadTimeLocation();
    tlo_save.setEnabled(false);

}



void ICACHE_FLASH_ATTR NetworkSettingsPage::handleAjax(){

    LOG_HIGH(PSTR("(Page) Network Settings - Handle AJAX"));

    if( website.AjaxID == F("wifi_stn_save") ) {
        saveWifiStation(website.AjaxValue.toInt());
        return;                                             // TODO - change all other handlers for this
    }
    
    // Used to send back basic details of a specific wifi station
    if( website.AjaxID == F("wifi_stn_id") ) {
        wifi_stn_name.setValue(config.settings.networkSettings.stationSettings[wifi_stn_id.intValue()].SSID);
        wifi_stn_on.setValue(network.isStationConnected(wifi_stn_id.intValue()));
        return;
    }

    if( website.AjaxID == F("wifi_stn_btn") ) {
        loadWifiStation(wifi_stn_btn.intValue());
        return;
    }

    if( website.AjaxID == F("wifi_stn_cnct") ) {
        connectWifiStation(wifi_stn_cnct.intValue());     
        return;
    }

    if( website.AjaxID == F("wifi_mode_save") ) {
        setWifiMode((WiFiMode)website.AjaxValue.toInt()); 
        return;
    }

    if( website.AjaxID == F("wifi_ap_save") ) {
        saveAP();
        return;
    }

    if( website.AjaxID == F("net_ck_save") ) {
        saveNetCheck();
        return;
    }

    if( website.AjaxID == F("dns_save") ) {
        saveDNS();
        return;
    }

    if( website.AjaxID == F("tlo_save") ) {
        saveTimeLocation();
        return;
    }

    if( website.AjaxID == F("tlo_detect") ) {
        detectLocation();
        return;
    }

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveDNS() {
    LOG_HIGH(PSTR("(Page) Network Settings - Save DNS"));

    DNSConfig dns;

    dns.mode = dns_mode.isChecked();
    strncpy(dns.hostname,dns_name.value(),DNS_MAX_HOSTNAME_LEN);
    dns.mDNS = dns_mdns.isChecked();

    config.settings.networkSettings.dnsSettings = dns;
    config.Save();

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveTimeLocation() {
    LOG_HIGH(PSTR("(Page) Network Settings - Save Time/Location"));

    strncpy(config.settings.timelocsettings.ipinfoToken,tlo_token.value(),TLO_IPINFO_MAX_TOKEN_LEN);
    config.settings.timelocsettings.ntpMode = tlo_ntp.isChecked();
    // Note location, if updated, is already saved to config.settings.tloconfig by timelocation.detectlocation
    config.Save();
}


void ICACHE_FLASH_ATTR NetworkSettingsPage::detectLocation() {
    if( timelocation.detectLocation() ) loadTimeLocation();
}


void ICACHE_FLASH_ATTR NetworkSettingsPage::loadTimeLocation() {

    if( timelocation.isLocationSet() ) {
        tlo_loc.setValue( config.settings.timelocsettings.location.region );
        tlo_tz.setValue( config.settings.timelocsettings.location.timezone );
    }
    else {
        tlo_loc.setValue( "<b>Not set</b>", true );                                                 // TODO - what to do about progmem
        tlo_tz.setValue( "<b>Not set</b> <span class=\"w3-small\">(Using UTC)</span>", true );
    }
}



void ICACHE_FLASH_ATTR NetworkSettingsPage::setWifiMode(WiFiMode mode) {
    LOG(PSTR("(Page) Network Settings - Set WiFi mode"));
    
    network.setWiFiMode(mode);
    config.Save();

    wifi_stn_asip.setValue(network.getAssignedIP());
    wifi_mode_stn.setChecked( mode == WIFI_STA || mode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( mode == WIFI_AP || mode == WIFI_AP_STA );
    clearLoader.call();
}



void ICACHE_FLASH_ATTR NetworkSettingsPage::loadWifiStation(uint id) {

    LOG_HIGH(PSTR("(Page) Network Settings - Load Wifi Station"));

    wifiStation = config.settings.networkSettings.stationSettings[id];

    wifi_stn_ssid.setValue(wifiStation.SSID);
    wifi_stn_pwd.setValue(wifiStation.password);
    wifi_stn_ip.setValue(wifiStation.ip.toString().c_str());
    wifi_stn_snet.setValue(wifiStation.subnet.toString().c_str());
    wifi_stn_gtwy.setValue(wifiStation.gateway.toString().c_str());
    wifi_stn_dns1.setValue(wifiStation.dns1.toString().c_str());
    wifi_stn_dns2.setValue(wifiStation.dns2.toString().c_str());
    wifi_stn_dhcp.setChecked(wifiStation.DHCPMode == DHCP);

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveAP() {

    LOG_HIGH(PSTR("(Page) Network Settings - Save AP"));
    
    APConfig ap;

    strncpy(ap.SSID, wifi_ap_ssid.value(), NET_MAX_SSID_LEN);
    strncpy(ap.password, wifi_ap_pwd.value(), NET_MAX_PASSWORD_LEN);
    
    bool valid = true;
    valid = valid && ap.ip.fromString(wifi_ap_ip.value());
    valid = valid && ap.subnet.fromString(wifi_ap_snet.value());
    valid = valid && ap.gateway.fromString(wifi_ap_gtwy.value());
    ap.channel = atoi(wifi_ap_ch.value());

    if( valid ) {
        config.settings.networkSettings.apSettings = ap;
        config.Save();

        network.reconnectWifi();
    }
    else {
        website.postMessage(PSTR("Invlaid IP address"));
    }

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveNetCheck() {

    LOG_HIGH(PSTR("(Page) Network Settings - Save Connection Checker"));

    NetCheckConfig netStatus;

    netStatus.mode = net_ck_mode.isChecked();
    strncpy(netStatus.checkService,net_ck_url.value(),NETCHECK_MAX_SERVICE_LEN);
    netStatus.interval = atoi(net_ck_int.value());

    config.settings.networkSettings.netCheckSettings = netStatus;
    config.Save();

    network.setNetChecker();

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveWifiStation(uint id) {

    LOG_HIGH(PSTR("(Page) Network Settings - Save Wifi Station"));

    strncpy(wifiStation.SSID, wifi_stn_ssid.value(), NET_MAX_SSID_LEN);
    strncpy(wifiStation.password, wifi_stn_pwd.value(), NET_MAX_PASSWORD_LEN);
 
    bool valid = true;
    valid = valid && wifiStation.ip.fromString(wifi_stn_ip.value());
    valid = valid && wifiStation.subnet.fromString(wifi_stn_snet.value());
    valid = valid && wifiStation.gateway.fromString(wifi_stn_gtwy.value());
    valid = valid && wifiStation.dns1.fromString(wifi_stn_dns2.value());
    valid = valid && wifiStation.dns2.fromString(wifi_stn_dns2.value());

    wifiStation.DHCPMode = (wifi_stn_dhcp.isChecked() ? DHCP : STATIC);

    if( valid || wifiStation.DHCPMode == DHCP ) {
        config.settings.networkSettings.stationSettings[id] = wifiStation;
        config.Save();

        if( network.getConnectedStation() == id ) network.reconnectWifi();    
    }
    else {
         website.postMessage(PSTR("Invlaid IP address"));
    }

    // Make the client reload the wifi list
    wifi_stn_asip.setValue(network.getAssignedIP());
    loadWifiList.call();

}

void ICACHE_FLASH_ATTR NetworkSettingsPage::connectWifiStation(uint id) {

    LOG_HIGH(PSTR("(Page) Network Settings - Connect Wifi Station"));
    
    if( network.connectWiFiStation(id) ) config.Save();
    else network.reconnectWifi();

    // Make the client reload the wifi list
    wifi_stn_asip.setValue(network.getAssignedIP());
    loadWifiList.call();

}


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETWORKSETTINGSPAGE)

    NetworkSettingsPage networksettingspage(
        []() { 
            networksettingspage.ajax.handleRequest( 
                []() {
                    networksettingspage.handleAjax();
                }
            ); 
        },
        []() { 
            networksettingspage.initializeAjax();
        }
    );

#endif