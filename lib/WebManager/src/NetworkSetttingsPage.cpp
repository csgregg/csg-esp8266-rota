/* Website Manager Library

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



#include "NetworkSetttingsPage.h"
#include "Logger.h"
#include "ConfigManager.h"

// TODO: Flashstrings
// TODO: AP mode
// TODO: Mode control

void NetworkSettingsPage::initializeAjax(){

    LOG_HIGH("(Page) Network Settings - Initialize AJAX");

    char ipbuffer[15];

    wifiAP = config.settings.networkConfig.apSettings;

    wifi_ap_ssid.setValue(wifiAP.SSID);
    wifi_ap_pwd.setValue(wifiAP.password);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiAP.ip[0], wifiAP.ip[1], wifiAP.ip[2], wifiAP.ip[3] );
    wifi_ap_ip.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiAP.subnet[0], wifiAP.subnet[1], wifiAP.subnet[2], wifiAP.subnet[3] );
    wifi_ap_snet.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiAP.gateway[0], wifiAP.gateway[1], wifiAP.gateway[2], wifiAP.gateway[3] );
    wifi_ap_gtwy.setValue(ipbuffer);

    wifi_ap_ch.selectOption(wifiAP.channel);

    wifi_stn_asip.setValue(getAssignedIP());

    WiFiMode mode =  config.settings.networkConfig.wifiMode;
    wifi_mode_stn.setChecked( mode == WIFI_STA || mode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( mode == WIFI_AP || mode == WIFI_AP_STA );

}



void NetworkSettingsPage::handleAjax(){

    LOG_HIGH("(Page) Network Settings - Handle AJAX");

    if( website.AjaxID == "wifi_stn_save" ) saveWifiStation(website.AjaxValue.toInt());
    
    // Used to send back basic details of a specific wifi station
    if( website.AjaxID == "wifi_stn_id" ) {
        wifi_stn_name.setValue(config.settings.networkConfig.stationSettings[wifi_stn_id.intValue()].SSID);
        wifi_stn_on.setValue(network.isStationConnected(wifi_stn_id.intValue()));
    }

    if( website.AjaxID == "wifi_stn_btn") loadWifiStation(wifi_stn_btn.intValue());

    if( website.AjaxID == "wifi_stn_cnct") connectWifiStation(wifi_stn_cnct.intValue());     

    if( website.AjaxID == "wifi_mode_save" ) setWifiMode((WiFiMode)website.AjaxValue.toInt()); 

    if( website.AjaxID == "wifi_ap_save" ) saveAP();

}


void NetworkSettingsPage::setWifiMode(WiFiMode mode) {
    LOG("(Page) Network Settings - Set WiFi mode");
    
    network.setWiFiMode(mode);
    config.Save();

    wifi_stn_asip.setValue(getAssignedIP());
    wifi_mode_stn.setChecked( mode == WIFI_STA || mode == WIFI_AP_STA );
    wifi_mode_ap.setChecked( mode == WIFI_AP || mode == WIFI_AP_STA );
    clearLoader.call(1);
}



void NetworkSettingsPage::loadWifiStation(uint id) {

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


void NetworkSettingsPage::saveAP() {
    
    APConfig ap;

    strncpy(ap.SSID, wifi_ap_ssid.value(), MAX_SSID_LEN);
    strncpy(ap.password, wifi_ap_pwd.value(), MAX_PASSWORD_LEN);
    
    // TODO - Add error checking
    ap.ip.fromString(wifi_ap_ip.value());

    ap.subnet.fromString(wifi_ap_snet.value());
    ap.gateway.fromString(wifi_ap_gtwy.value());
    ap.channel = atoi(wifi_ap_ch.value());

    config.settings.networkConfig.apSettings = ap;
    config.Save();

    network.reconnectWifi();
}


void NetworkSettingsPage::saveWifiStation(uint id) {

    strncpy(wifiStation.SSID, wifi_stn_ssid.value(), MAX_SSID_LEN);
    strncpy(wifiStation.password, wifi_stn_pwd.value(), MAX_PASSWORD_LEN);
    
    // TODO - Add error checking
    wifiStation.ip.fromString(wifi_stn_ip.value());

    wifiStation.subnet.fromString(wifi_stn_snet.value());
    wifiStation.gateway.fromString(wifi_stn_gtwy.value());
    wifiStation.dns1.fromString(wifi_stn_dns2.value());
    wifiStation.dns2.fromString(wifi_stn_dns2.value());

    wifiStation.DHCPMode = (wifi_stn_dhcp.isChecked() ? DHCP : STATIC);

    config.settings.networkConfig.stationSettings[id] = wifiStation;
    config.Save();

    if( network.getConnectedStation() == id ) network.reconnectWifi();

    // Make the client reload the wifi list
    wifi_stn_asip.setValue(getAssignedIP());
    loadWifiList.call(1);

}

void NetworkSettingsPage::connectWifiStation(uint id) {
    
    if( network.connectWiFiStation(id) ) config.Save();
    else network.reconnectWifi();

    // Make the client reload the wifi list
    wifi_stn_asip.setValue(getAssignedIP());
    loadWifiList.call(1);

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