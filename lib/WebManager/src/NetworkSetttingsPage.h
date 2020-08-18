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

Describes the Network Settings web page

*/


#ifndef NETWORK_SETTINGS_PAGE_H

    #define NETWORK_SETTINGS_PAGE_H

    #include "EmbAJAX.h"
    #include "WebManager.h"
    #include "NetworkManager.h"


    class NetworkSettingsPage {
        public:                             // TODO - sort out public and protected

            const char* URL;
            void (*handler)();
            void (*init)();


            StationConfig wifiStation;
            APConfig wifiAP;

            // Wifi AP Elements

            EmbAJAXCheckButton wifi_mode_ap;
            EmbAJAXCheckButton wifi_mode_stn;
            EmbAJAXServerFunction wifi_mode_save;

            EmbAJAXServerFunction wifi_ap_save;
            EmbAJAXTextInput<32> wifi_ap_ssid;
            EmbAJAXTextInput<16> wifi_ap_pwd;
            EmbAJAXTextInput<15> wifi_ap_ip;
            EmbAJAXTextInput<15> wifi_ap_snet;
            EmbAJAXTextInput<15> wifi_ap_gtwy;
            EmbAJAXOptionSelect<13> wifi_ap_ch;
            const char* wifi_chs[13] = {"1","2","3","4","5","6","7","8","9","10","11","12","13"};
            

            // Wifi Station Elements

            EmbAJAXServerFunction wifi_stn_save;
            EmbAJAXServerFunction wifi_stn_id;
            EmbAJAXServerFunction wifi_stn_btn;     // Are these really functions or are they varilables
            EmbAJAXServerFunction wifi_stn_cnct;

            EmbAJAXVarInt wifi_stn_count;
            EmbAJAXVarChar<32> wifi_stn_name;
            EmbAJAXVarBool wifi_stn_on;
            EmbAJAXClientFunction<int> loadWifiList;

            EmbAJAXTextInput<32> wifi_stn_ssid;
            EmbAJAXTextInput<16> wifi_stn_pwd;
            EmbAJAXCheckButton wifi_stn_dhcp;
            EmbAJAXTextInput<15> wifi_stn_ip;
            EmbAJAXTextInput<15> wifi_stn_snet;
            EmbAJAXTextInput<15> wifi_stn_gtwy;
            EmbAJAXTextInput<15> wifi_stn_dns1;
            EmbAJAXTextInput<15> wifi_stn_dns2;
            EmbAJAXMutableSpan wifi_stn_asip;

            EmbAJAXClientFunction<int> clearLoader;

            EmbAJAXBase* page_elements[28] = {

            &wifi_mode_ap,
            &wifi_mode_stn,
            &wifi_mode_save,

            &wifi_ap_save,
            &wifi_ap_ssid,
            &wifi_ap_pwd,
            &wifi_ap_ip,
            &wifi_ap_snet,
            &wifi_ap_gtwy,
            &wifi_ap_ch,

            &wifi_stn_save,
            &wifi_stn_id,
            &wifi_stn_btn,
            &wifi_stn_cnct,

            &wifi_stn_count,
            &wifi_stn_name,
            &wifi_stn_on,
            &loadWifiList,

            &wifi_stn_ssid,
            &wifi_stn_pwd,
            &wifi_stn_dhcp,
            &wifi_stn_ip,
            &wifi_stn_snet,
            &wifi_stn_gtwy,
            &wifi_stn_dns1,
            &wifi_stn_dns2,
            &wifi_stn_asip,

            &clearLoader,

            };

            NetworkSettingsPage( void(*phandler)(), void(*pinit)() ) : 

                wifi_mode_ap("wifi_mode_ap",""),
                wifi_mode_stn("wifi_mode_stn",""),
                wifi_mode_save("wifi_mode_save"),
            
                wifi_ap_save("wifi_ap_save"),
                wifi_ap_ssid("wifi_ap_ssid"),
                wifi_ap_pwd("wifi_ap_pwd"),
                wifi_ap_ip("wifi_ap_ip"),
                wifi_ap_snet("wifi_ap_snet"),
                wifi_ap_gtwy("wifi_ap_gtwy"),
                wifi_ap_ch("wifi_ap_ch",wifi_chs),

                wifi_stn_save("wifi_stn_save"),
                wifi_stn_id("wifi_stn_id"),
                wifi_stn_btn("wifi_stn_btn"),
                wifi_stn_cnct("wifi_stn_cnct"),
                    
                wifi_stn_count("wifi_stn_count",MAX_SSIDS),
                wifi_stn_name("wifi_stn_name",""),
                wifi_stn_on("wifi_stn_on",false),
                loadWifiList("loadWifiList"),

                wifi_stn_ssid("wifi_stn_ssid"),
                wifi_stn_pwd("wifi_stn_pwd"),
                wifi_stn_dhcp("wifi_stn_dhcp",""),
                wifi_stn_ip("wifi_stn_ip"),
                wifi_stn_snet("wifi_stn_snet"),
                wifi_stn_gtwy("wifi_stn_gtwy"),
                wifi_stn_dns1("wifi_stn_dns1"),
                wifi_stn_dns2("wifi_stn_dns2"),
                wifi_stn_asip("wifi_stn_asip"),

                clearLoader("clearLoader"),

                ajax(page_elements, "")
                {
                    URL = "/netsettings.html";
                    handler = phandler;
                    init = pinit;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

            void handleAjax();

            void initializeAjax();

            void loadWifiStation(uint id);

            void saveWifiStation(uint id);

            void connectWifiStation(uint id);

            void setWifiMode(WiFiMode mode);

            void saveAP();

            char* getAssignedIP() {
                static char ip[15];
                sprintf(ip, "%i.%i.%i.%i", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
                return ip;
            }

    };
    
    extern NetworkSettingsPage networksettingspage;

#endif