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
#include "website.h"

void NetworkSettingsPage::initializeAjax(){

    LOG("Initialize Network Settings AJAX");

    wifi_stn1_btn.setValue(config.settings.networkConfig.stationSettings[0].SSID);
    wifi_stn2_btn.setValue(config.settings.networkConfig.stationSettings[1].SSID);
    wifi_stn3_btn.setValue(config.settings.networkConfig.stationSettings[2].SSID);

    wifi_stn1_ctrl.setValue(network.stationConnected[0]?"On":"Off");
    wifi_stn2_ctrl.setValue(network.stationConnected[1]?"On":"Off");
    wifi_stn3_ctrl.setValue(network.stationConnected[2]?"On":"Off");

    strcpy(buff,"hello");

    wifi_var_test.setValue(buff);
    wifi_var_count.setValue(0);

}

void NetworkSettingsPage::handleAjax(){

    LOG("Handle Network Settings AJAX");

    if( website.AjaxID == "wifi_stn1_btn" ||
        website.AjaxID == "wifi_stn2_btn" ||
        website.AjaxID == "wifi_stn3_btn" )
        loadWifiStation(website.AjaxValue.toInt());

    if( website.AjaxID == "wifi_stn_save" || website.AjaxID == "wifi_stn_forget" ) saveWifiStation(website.AjaxValue.toInt());
    
    DEBUG(wifi_var_test.getValue());

    int i = wifi_var_count.getValue();
    char buff[8];
    itoa(i,buff,10);
    DEBUG(buff);

    wifi_test_console.call("Hi there");
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
    wifi_stn_subnet.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.gateway[0], wifiStation.gateway[1], wifiStation.gateway[2], wifiStation.gateway[3] );
    wifi_stn_gateway.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns1[0], wifiStation.dns1[1], wifiStation.dns1[2], wifiStation.dns1[3] );
    wifi_stn_dns1.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns2[0], wifiStation.dns2[1], wifiStation.dns2[2], wifiStation.dns2[3] );
    wifi_stn_dns2.setValue(ipbuffer);

    wifi_stn_dhcp.setChecked(wifiStation.DHCPMode == DHCP);

}


void NetworkSettingsPage::saveWifiStation(uint id) {

    strncpy(wifiStation.SSID, wifi_stn_ssid.value(), MAX_SSID_LEN);
    strncpy(wifiStation.password, wifi_stn_pwd.value(), MAX_PASSWORD_LEN);
    
    // TODO - Add error checking
    wifiStation.ip.fromString(wifi_stn_ip.value());

    wifiStation.subnet.fromString(wifi_stn_subnet.value());
    wifiStation.gateway.fromString(wifi_stn_gateway.value());
    wifiStation.dns1.fromString(wifi_stn_dns2.value());
    wifiStation.dns2.fromString(wifi_stn_dns2.value());

    wifiStation.DHCPMode = (wifi_stn_dhcp.isChecked() ? DHCP : STATIC);

    config.settings.networkConfig.stationSettings[id] = wifiStation;
    config.Save();

    if( network.ConnectedStation == id ) network.reconnectWifi();

    initializeAjax();
}



void NetworkSettingsPage::forgetWifiStation(uint id) {

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