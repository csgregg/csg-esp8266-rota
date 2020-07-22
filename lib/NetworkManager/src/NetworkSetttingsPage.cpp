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

    // TODO - is this needed?
    if( isInitialized ) return;
    isInitialized = true;

    DEBUG("Initialize Network Settings AJAX");

    String stationConnected = network.stationConnected[0] ? " - Connected" : "Not connected";

    DEBUG(stationConnected);

    wifi_station_1.setValue(config.settings.networkConfig.stationSettings[0].SSID);
    wifi_station_2.setValue(config.settings.networkConfig.stationSettings[1].SSID);
    wifi_station_3.setValue(config.settings.networkConfig.stationSettings[2].SSID);

}

void NetworkSettingsPage::handleAjax(){

    DEBUG("Handle Network Settings AJAX");

    DEBUG(website.URL);
    DEBUG(website.AjaxID);
    DEBUG(website.AjaxValue);

    if( website.AjaxID == "wifi_station_1" ||
        website.AjaxID == "wifi_station_2" ||
        website.AjaxID == "wifi_station_3" )
        loadWifiStation(website.AjaxValue.toInt());

    if( website.AjaxID == "save_button" ) saveWifiStation(website.AjaxValue.toInt());
    
}

void NetworkSettingsPage::loadWifiStation(uint id) {

    DEBUG("Load");
    char ipbuffer[15];

    wifiStation = config.settings.networkConfig.stationSettings[id];

    wifi_ssid.setValue(wifiStation.SSID);
    wifi_password.setValue(wifiStation.password);

    // IP Addresses
    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.ip[0], wifiStation.ip[1], wifiStation.ip[2], wifiStation.ip[3] );
    wifi_ip.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.subnet[0], wifiStation.subnet[1], wifiStation.subnet[2], wifiStation.subnet[3] );
    wifi_subnet.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.gateway[0], wifiStation.gateway[1], wifiStation.gateway[2], wifiStation.gateway[3] );
    wifi_gateway.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns1[0], wifiStation.dns1[1], wifiStation.dns1[2], wifiStation.dns1[3] );
    wifi_dns1.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns2[0], wifiStation.dns2[1], wifiStation.dns2[2], wifiStation.dns2[3] );
    wifi_dns2.setValue(ipbuffer);

    wifi_dhcp_mode.setChecked(wifiStation.DHCPMode == DHCP);

}


void NetworkSettingsPage::saveWifiStation(uint id) {

    DEBUG("Saving");

    strncpy(wifiStation.SSID, wifi_ssid.value(), MAX_SSID_LEN);
    strncpy(wifiStation.password, wifi_password.value(), MAX_PASSWORD_LEN);
    
    wifiStation.ip.fromString(wifi_ip.value());

    wifiStation.subnet.fromString(wifi_subnet.value());
    wifiStation.gateway.fromString(wifi_gateway.value());
    wifiStation.dns1.fromString(wifi_dns2.value());
    wifiStation.dns2.fromString(wifi_dns2.value());

    wifiStation.DHCPMode = (wifi_dhcp_mode.isChecked() ? DHCP : STATIC);

    config.settings.networkConfig.stationSettings[id] = wifiStation;
    config.Save();

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