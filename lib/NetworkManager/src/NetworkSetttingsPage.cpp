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
    
    if( isInitialized ) return;
    isInitialized = true;

    DEBUG("Initialize Network Settings AJAX");

    char ipbuffer[15];

    // WiFi 1
    // ======

    wifiStation = config.settings.networkConfig.stationSettings[0];

    wifi1_ssid.setValue(wifiStation.SSID);
    wifi1_password.setValue(wifiStation.password);

    // IP Addresses
    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.ip[0], wifiStation.ip[1], wifiStation.ip[2], wifiStation.ip[3] );
    wifi1_ip.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.subnet[0], wifiStation.subnet[1], wifiStation.subnet[2], wifiStation.subnet[3] );
    wifi1_subnet.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.gateway[0], wifiStation.gateway[1], wifiStation.gateway[2], wifiStation.gateway[3] );
    wifi1_gateway.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns1[0], wifiStation.dns1[1], wifiStation.dns1[2], wifiStation.dns1[3] );
    wifi1_dns1.setValue(ipbuffer);

    sprintf(ipbuffer, "%i.%i.%i.%i", wifiStation.dns2[0], wifiStation.dns2[1], wifiStation.dns2[2], wifiStation.dns2[3] );
    wifi1_dns2.setValue(ipbuffer);

    // DHCP Mode
    bool dhcp = (wifiStation.DHCPMode == DHCP);
    wifi1_dhcp_mode.setChecked(dhcp);
    wifi1_dhcp_mode_label.setValue(dhcp ? "Dynamic IP" : "Static IP");
    wifi1_static_show.setVisible(!dhcp);

    save_button.setEnabled(isChanged);

}

void NetworkSettingsPage::handleAjax(){

        DEBUG("Handle Network Settings AJAX");

        DEBUG(website.URL);
        DEBUG(website.AjaxID);
        DEBUG(website.AjaxValue);

        isChanged = true;

        // WiFi 1
        // ======

        wifiStation = config.settings.networkConfig.stationSettings[0];

        strncpy(wifiStation.SSID, wifi1_ssid.value(), MAX_SSID_LEN);
        strncpy(wifiStation.password, wifi1_password.value(), MAX_PASSWORD_LEN);
        
        wifiStation.ip.fromString(wifi1_ip.value());

        wifiStation.subnet.fromString(wifi1_subnet.value());
        wifiStation.gateway.fromString(wifi1_gateway.value());
        wifiStation.dns1.fromString(wifi1_dns2.value());
        wifiStation.dns2.fromString(wifi1_dns2.value());

        // DHCP Mode
        bool dhcp = wifi1_dhcp_mode.isChecked();
        wifiStation.DHCPMode = (dhcp ? DHCP : STATIC);
        wifi1_dhcp_mode_label.setValue(dhcp ? "Dynamic IP" : "Static IP");
        wifi1_static_show.setVisible(!dhcp);

        save_button.setEnabled(isChanged);

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