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


#ifndef NETWORK_SETTINGS_PAGE_H

    #define NETWORK_SETTINGS_PAGE_H

    #include "WebManager.h"
    #include "NetworkManager.h"


    class NetworkSettingsPage {

        public:

            const char* URL;
            void (*handler)();
            void (*init)();


            StationConfig wifiStation;

            // Wifi AP Elements

            EmbAJAXCheckButton wifi_mode_ap;
            EmbAJAXCheckButton wifi_mode_stn;
            EmbAJAXServerFunction wifi_mode_save;

            EmbAJAXServerFunction wifi_ap_save;
            EmbAJAXTextInput<NET_MAX_SSID_LEN> wifi_ap_ssid;
            EmbAJAXTextInput<NET_MAX_PASSWORD_LEN> wifi_ap_pwd;
            EmbAJAXTextInput<16> wifi_ap_ip;
            EmbAJAXTextInput<16> wifi_ap_snet;
            EmbAJAXTextInput<16> wifi_ap_gtwy;
            EmbAJAXOptionSelect<13> wifi_ap_ch;
            const char* wifi_chs[13] = {"1","2","3","4","5","6","7","8","9","10","11","12","13"};
            

            // Wifi Station Elements

            EmbAJAXServerFunction wifi_stn_save;
            EmbAJAXServerFunction wifi_stn_id;
            EmbAJAXServerFunction wifi_stn_btn;
            EmbAJAXServerFunction wifi_stn_cnct;

            EmbAJAXVarInt wifi_stn_count;
            EmbAJAXVarChar<NET_MAX_SSID_LEN> wifi_stn_name;
            EmbAJAXVarBool wifi_stn_on;
            EmbAJAXClientFunction loadWifiList;

            EmbAJAXTextInput<NET_MAX_SSID_LEN> wifi_stn_ssid;
            EmbAJAXTextInput<NET_MAX_PASSWORD_LEN> wifi_stn_pwd;
            EmbAJAXCheckButton wifi_stn_dhcp;
            EmbAJAXTextInput<16> wifi_stn_ip;
            EmbAJAXTextInput<16> wifi_stn_snet;
            EmbAJAXTextInput<16> wifi_stn_gtwy;
            EmbAJAXTextInput<16> wifi_stn_dns1;
            EmbAJAXTextInput<16> wifi_stn_dns2;
            EmbAJAXMutableSpan wifi_stn_asip;

            EmbAJAXClientFunction clearLoader;

            EmbAJAXCheckButton net_ck_mode;
            EmbAJAXTextInput<4> net_ck_int;
            EmbAJAXTextInput<NETCHECK_MAX_SERVICE_LEN> net_ck_url;
            EmbAJAXServerFunction net_ck_save;

            EmbAJAXCheckButton dns_mode;
            EmbAJAXCheckButton dns_mdns;
            EmbAJAXTextInput<DNS_MAX_HOSTNAME_LEN> dns_name;
            EmbAJAXServerFunction dns_save;

            EmbAJAXServerFunction tlo_detect;
            EmbAJAXTextInput<TLO_IPINFO_MAX_TOKEN_LEN> tlo_token;
            EmbAJAXMutableSpan tlo_loc;
            EmbAJAXMutableSpan tlo_tz;
            EmbAJAXCheckButton tlo_ntp;
            EmbAJAXServerFunction tlo_save;

            EmbAJAXBase* page_elements[WEB_PAGE_COMMON_ELEMENTS_COUNT + 42] = {

            WEB_PAGE_COMMON_ELEMENTS,

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

            &net_ck_mode,
            &net_ck_int,
            &net_ck_url,
            &net_ck_save,

            &dns_mode,
            &dns_mdns,
            &dns_name,
            &dns_save,

            &tlo_detect,
            &tlo_token,
            &tlo_loc,
            &tlo_tz,
            &tlo_ntp,
            &tlo_save,

            };

            NetworkSettingsPage( void(*phandler)(), void(*pinit)() ) : 
               
                wifi_mode_ap("wifi_mode_ap",""),            // TODO - should all these strings be literals in PROGMEM
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
                    
                wifi_stn_count("wifi_stn_count",NET_MAX_SSIDS),
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

                net_ck_mode("net_ck_mode",""),
                net_ck_int("net_ck_int"),
                net_ck_url("net_ck_url"),
                net_ck_save("net_ck_save"),

                dns_mode("dns_mode",""),
                dns_mdns("dns_mdns",""),
                dns_name("dns_name"),
                dns_save("dns_save"),

                tlo_detect("tlo_detect"),
                tlo_token("tlo_token"),
                tlo_loc("tlo_loc"),
                tlo_tz("tlo_tz"),
                tlo_ntp("tlo_ntp",""),
                tlo_save("tlo_save"),

                ajax(page_elements, "")
                {
                    URL = "/netsettings.html";
                    handler = phandler;
                    init = pinit;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

            void ICACHE_FLASH_ATTR handleAjax();

            void ICACHE_FLASH_ATTR initializeAjax();

            void ICACHE_FLASH_ATTR loadWifiStation(uint id);

            void ICACHE_FLASH_ATTR saveWifiStation(uint id);

            void ICACHE_FLASH_ATTR connectWifiStation(uint id);

            void ICACHE_FLASH_ATTR setWifiMode(WiFiMode mode);

            void ICACHE_FLASH_ATTR saveAP();

            void ICACHE_FLASH_ATTR saveNetCheck();

            void ICACHE_FLASH_ATTR saveDNS();

            void ICACHE_FLASH_ATTR saveTimeLocation();
            void ICACHE_FLASH_ATTR loadTimeLocation();
            void ICACHE_FLASH_ATTR detectLocation();

    };
    
    extern NetworkSettingsPage networksettingspage;

#endif