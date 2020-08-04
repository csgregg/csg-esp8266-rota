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


#ifndef NETWORK_SETTINGS_PAGE2_H

    #define NETWORK_SETTINGS_PAGE2_H

    #include "EmbAJAX.h"
    #include "NetworkManager.h"
    #include "ConfigManager.h"
    #include "Logger.h"
    #include "website.h"


    class NetworkSettingsPage2 {
    public:

        const char* URL;
        void (*handler)();
        void (*init)();

        StationConfig wifiStation;

        EmbAJAXMutableSpan wifi_stn_save;

        EmbAJAXVariable<int> wifi_stn_count;
        EmbAJAXVariable<int> wifi_stn_max;
        EmbAJAXVarChar<32> wifi_stn_name;
        EmbAJAXVariable<int> wifi_stn_on;
        EmbAJAXVariable<int> wifi_stn_id;
        EmbAJAXVariable<int> wifi_stn_btn;

        EmbAJAXTextInput<32> wifi_stn_ssid;
        EmbAJAXTextInput<16> wifi_stn_pwd;
        EmbAJAXCheckButton wifi_stn_dhcp;
        EmbAJAXTextInput<15> wifi_stn_ip;
        EmbAJAXTextInput<15> wifi_stn_subnet;
        EmbAJAXTextInput<15> wifi_stn_gateway;
        EmbAJAXTextInput<15> wifi_stn_dns1;
        EmbAJAXTextInput<15> wifi_stn_dns2;

        EmbAJAXBase* page_elements[15] = {

        &wifi_stn_save,

        &wifi_stn_count,
        &wifi_stn_max,
        &wifi_stn_name,
        &wifi_stn_on,
        &wifi_stn_id,
        &wifi_stn_btn,

        &wifi_stn_ssid,
        &wifi_stn_pwd,
        &wifi_stn_dhcp,
        &wifi_stn_ip,
        &wifi_stn_subnet,
        &wifi_stn_gateway,
        &wifi_stn_dns1,
        &wifi_stn_dns2

        };

        NetworkSettingsPage2( void(*phandler)(), void(*pinit)() ) : 

            wifi_stn_save("wifi_stn_save"),
                   
            wifi_stn_count("wifi_stn_count",0),
            wifi_stn_max("wifi_stn_max",MAX_SSIDS),
            wifi_stn_name("wifi_stn_name",""),
            wifi_stn_on("wifi_stn_on",0),
            wifi_stn_id("wifi_stn_id",0),
            wifi_stn_btn("wifi_stn_btn",0),

            wifi_stn_ssid("wifi_stn_ssid"),
            wifi_stn_pwd("wifi_stn_pwd"),
            wifi_stn_dhcp("wifi_stn_dhcp",""),
            wifi_stn_ip("wifi_stn_ip"),
            wifi_stn_subnet("wifi_stn_subnet"),
            wifi_stn_gateway("wifi_stn_gateway"),
            wifi_stn_dns1("wifi_stn_dns1"),
            wifi_stn_dns2("wifi_stn_dns2"),

            ajax(page_elements, "")
            {
                URL = "/networksettings2.html";
                handler = phandler;
                init = pinit;
            };

        EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

        void handleAjax();

        void initializeAjax();

        void loadWifiStation(uint id);

        void saveWifiStation(uint id);

        char buff[32];

    };
    
    extern NetworkSettingsPage2 networksettingspage2;

#endif