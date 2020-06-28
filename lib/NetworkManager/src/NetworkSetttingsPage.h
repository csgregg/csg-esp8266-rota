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
    #include "NetworkManager.h"
    #include "ConfigManager.h"
    #include "Logger.h"

    struct NetworkSettingsPage {

        const char* URL;
        void (*handler)();
        void (*init)();

        bool isInitialized = false;   
        bool isChanged = false; 
        StationConfig wifiStation;

        EmbAJAXMutableSpan save_button;

        EmbAJAXTextInput<32> wifi1_ssid;
        EmbAJAXTextInput<16> wifi1_password;
        EmbAJAXCheckButton wifi1_dhcp_mode;
        EmbAJAXMutableSpan wifi1_dhcp_mode_label;
        EmbAJAXHideableContainer<0> wifi1_static_show;
        EmbAJAXTextInput<15> wifi1_ip;
        EmbAJAXTextInput<15> wifi1_subnet;
        EmbAJAXTextInput<15> wifi1_gateway;
        EmbAJAXTextInput<15> wifi1_dns1;
        EmbAJAXTextInput<15> wifi1_dns2;

        EmbAJAXBase* page_elements[11] = {
            &save_button,
            &wifi1_ssid,
            &wifi1_password,
            &wifi1_dhcp_mode,
            &wifi1_dhcp_mode_label,
            &wifi1_static_show,
            &wifi1_ip,
            &wifi1_subnet,
            &wifi1_gateway,
            &wifi1_dns1,
            &wifi1_dns2
        };

        NetworkSettingsPage( void(*phandler)(), void(*pinit)() ) : 
            save_button("save_button"),
            wifi1_ssid("wifi1_ssid"),
            wifi1_password("wifi1_password"),
            wifi1_dhcp_mode("wifi1_dhcp_mode", ""),
            wifi1_dhcp_mode_label("wifi1_dhcp_mode_label"),
            wifi1_static_show("wifi1_static_show",NULL),
            wifi1_ip("wifi1_ip"),
            wifi1_subnet("wifi1_subnet"),
            wifi1_gateway("wifi1_gateway"),
            wifi1_dns1("wifi1_dns1"),
            wifi1_dns2("wifi1_dns2"),
            ajax(page_elements, "")
            {
                URL = "/settings_wifi.html";
                handler = phandler;
                init = pinit;
            };

        EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

        void handleAjax();

        void initializeAjax();

    };
    
    extern NetworkSettingsPage networksettingspage;

#endif