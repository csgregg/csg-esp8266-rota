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

    char buffer[16];

    wifi_stn_asip.setValue(getAssignedIP());

    // AP Settings
    APConfig wifiAP = config.settings.networkConfig.apSettings;

    wifi_ap_ssid.setValue(wifiAP.SSID);
    wifi_ap_pwd.setValue(wifiAP.password);

    sprintf(buffer, "%i.%i.%i.%i", wifiAP.ip[0], wifiAP.ip[1], wifiAP.ip[2], wifiAP.ip[3] );
    wifi_ap_ip.setValue(buffer);

    sprintf(buffer, "%i.%i.%i.%i", wifiAP.subnet[0], wifiAP.subnet[1], wifiAP.subnet[2], wifiAP.subnet[3] );
    wifi_ap_snet.setValue(buffer);

    sprintf(buffer, "%i.%i.%i.%i", wifiAP.gateway[0], wifiAP.gateway[1], wifiAP.gateway[2], wifiAP.gateway[3] );
    wifi_ap_gtwy.setValue(buffer);

    wifi_ap_ch.selectOption(wifiAP.channel);

    wifi_ap_save.setEnabled(false);

    WiFiMode wifimode =  config.settings.networkConfig.wifiMode;
    wifi_mode_stn.setChecked( wifimode == WIFI_STA || wifimode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( wifimode == WIFI_AP || wifimode == WIFI_AP_STA );

    // Connectivity Settings
    NetCheckConfig netStatus = config.settings.networkConfig.netCheckSettings;
    net_ck_mode.setChecked( netStatus.mode );
    net_ck_int.setValue( itoa(netStatus.interval,buffer,10) );
    net_ck_url.setValue( netStatus.checkService );
    net_ck_save.setEnabled(false);

}



void ICACHE_FLASH_ATTR NetworkSettingsPage::handleAjax(){

    LOG_HIGH(PSTR("(Page) Network Settings - Handle AJAX"));

    if( website.AjaxID == F("wifi_stn_save") ) saveWifiStation(website.AjaxValue.toInt());
    
    // Used to send back basic details of a specific wifi station
    if( website.AjaxID == F("wifi_stn_id") ) {
        wifi_stn_name.setValue(config.settings.networkConfig.stationSettings[wifi_stn_id.intValue()].SSID);
        wifi_stn_on.setValue(network.isStationConnected(wifi_stn_id.intValue()));
    }

    if( website.AjaxID == F("wifi_stn_btn") ) loadWifiStation(wifi_stn_btn.intValue());

    if( website.AjaxID == F("wifi_stn_cnct") ) connectWifiStation(wifi_stn_cnct.intValue());     

    if( website.AjaxID == F("wifi_mode_save") ) setWifiMode((WiFiMode)website.AjaxValue.toInt()); 

    if( website.AjaxID == F("wifi_ap_save") ) saveAP();

    if( website.AjaxID == F("net_ck_save") ) saveNetCheck();

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::setWifiMode(WiFiMode mode) {
    LOG(PSTR("(Page) Network Settings - Set WiFi mode"));
    
    network.setWiFiMode(mode);
    config.Save();

    wifi_stn_asip.setValue(getAssignedIP());
    wifi_mode_stn.setChecked( mode == WIFI_STA || mode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( mode == WIFI_AP || mode == WIFI_AP_STA );
    clearLoader.call();
}



void ICACHE_FLASH_ATTR NetworkSettingsPage::loadWifiStation(uint id) {

    LOG_HIGH(F("(Page) Network Settings - Load Wifi Station"));

    char ipbuffer[15];

    wifiStation = config.settings.networkConfig.stationSettings[id];

    wifi_stn_ssid.setValue(wifiStation.SSID);
    wifi_stn_pwd.setValue(wifiStation.password);

    // IP Addresses
    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.ip[0], wifiStation.ip[1], wifiStation.ip[2], wifiStation.ip[3] );
    wifi_stn_ip.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.subnet[0], wifiStation.subnet[1], wifiStation.subnet[2], wifiStation.subnet[3] );
    wifi_stn_snet.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.gateway[0], wifiStation.gateway[1], wifiStation.gateway[2], wifiStation.gateway[3] );
    wifi_stn_gtwy.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns1[0], wifiStation.dns1[1], wifiStation.dns1[2], wifiStation.dns1[3] );
    wifi_stn_dns1.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns2[0], wifiStation.dns2[1], wifiStation.dns2[2], wifiStation.dns2[3] );
    wifi_stn_dns2.setValue(ipbuffer);

    wifi_stn_dhcp.setChecked(wifiStation.DHCPMode == DHCP);

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveAP() {

    LOG_HIGH(F("(Page) Network Settings - Save AP"));
    
    APConfig ap;

    strncpy(ap.SSID, wifi_ap_ssid.value(), MAX_SSID_LEN);
    strncpy(ap.password, wifi_ap_pwd.value(), MAX_PASSWORD_LEN);
    
    bool valid = true;
    valid = valid && ap.ip.fromString(wifi_ap_ip.value());
    valid = valid && ap.subnet.fromString(wifi_ap_snet.value());
    valid = valid && ap.gateway.fromString(wifi_ap_gtwy.value());
    ap.channel = atoi(wifi_ap_ch.value());

    if( valid ) {
        config.settings.networkConfig.apSettings = ap;
        config.Save();

        network.reconnectWifi();
    }
    else {
        website.postMessage(PSTR("Invlaid IP address"));
    }

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveNetCheck() {

    LOG_HIGH(F("(Page) Network Settings - Save Connection Checker"));

    NetCheckConfig netStatus;

    netStatus.mode = net_ck_mode.isChecked();
    strncpy(netStatus.checkService,net_ck_url.value(),MAX_CHECK_SERVICE_LEN);
    netStatus.interval = atoi(net_ck_int.value());

    config.settings.networkConfig.netCheckSettings = netStatus;
    config.Save();

    network.setNetChecker();

}


void ICACHE_FLASH_ATTR NetworkSettingsPage::saveWifiStation(uint id) {

    LOG_HIGH(F("(Page) Network Settings - Save Wifi Station"));

    strncpy(wifiStation.SSID, wifi_stn_ssid.value(), MAX_SSID_LEN);
    strncpy(wifiStation.password, wifi_stn_pwd.value(), MAX_PASSWORD_LEN);
 
    bool valid = true;
    valid = valid && wifiStation.ip.fromString(wifi_stn_ip.value());
    valid = valid && wifiStation.subnet.fromString(wifi_stn_snet.value());
    valid = valid && wifiStation.gateway.fromString(wifi_stn_gtwy.value());
    valid = valid && wifiStation.dns1.fromString(wifi_stn_dns2.value());
    valid = valid && wifiStation.dns2.fromString(wifi_stn_dns2.value());

    wifiStation.DHCPMode = (wifi_stn_dhcp.isChecked() ? DHCP : STATIC);

    if( valid || wifiStation.DHCPMode == DHCP ) {
        config.settings.networkConfig.stationSettings[id] = wifiStation;
        config.Save();

        if( network.getConnectedStation() == id ) network.reconnectWifi();    
    }
    else {
         website.postMessage(PSTR("Invlaid IP address"));
    }

    // Make the client reload the wifi list
    wifi_stn_asip.setValue(getAssignedIP());
    loadWifiList.call();

}

void ICACHE_FLASH_ATTR NetworkSettingsPage::connectWifiStation(uint id) {

    LOG_HIGH(F("(Page) Network Settings - Connect Wifi Station"));
    
    if( network.connectWiFiStation(id) ) config.Save();
    else network.reconnectWifi();

    // Make the client reload the wifi list
    wifi_stn_asip.setValue(getAssignedIP());
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