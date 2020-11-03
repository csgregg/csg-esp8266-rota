/**
 * @file        NetworkSettingsPage.h
 * @author      Chris Gregg
 * 
 * @brief       Server-side functions of netsettings.html
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

/* MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software" ), to deal
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
SOFTWARE. */


#ifndef NETWORK_SETTINGS_PAGE_H

    #define NETWORK_SETTINGS_PAGE_H

    // Global Libraries
    #include <Arduino.h>

    // Project Libraries
    #include "WebManager.h"
    #include "NetworkManager.h"

    /** @class NetworkSettingsPage
     *  @brief Server-side functions for netsettings.html page */
    class NetworkSettingsPage {                                                 // TODO - Rename to NetworkPage

        public:

            PageHandler handler;                // Handler for this page

            StationSettings wifiStation;        // Holder for WiFi stations settings

            // Wifi AP Elements

            EmbAJAXCheckButton wifi_mode_ap;                        // Check box for AP mode
            EmbAJAXCheckButton wifi_mode_stn;                       // Check box for station mode
            EmbAJAXServerFunction wifi_mode_save;                   // WiFi mode save button

            EmbAJAXServerFunction wifi_ap_save;                     // AP settings save button
            EmbAJAXTextInput<NET_MAX_SSID_LEN> wifi_ap_ssid;        // Input box for AP SSID
            EmbAJAXTextInput<NET_MAX_PASSWORD_LEN> wifi_ap_pwd;     // Input box for AP password
            EmbAJAXTextInput<16> wifi_ap_ip;                        // Input box for AP IP address
            EmbAJAXTextInput<16> wifi_ap_snet;                      // Input box for AP subnet
            EmbAJAXTextInput<16> wifi_ap_gtwy;                      // Input box for AP gateway
            EmbAJAXOptionSelect<13> wifi_ap_ch;                     // Dropdown select for AP channel
            const char* wifi_chs[13] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13"};       // AP channel options for dropdown
            
            // Wifi Station Elements

            EmbAJAXServerFunction wifi_stn_save;                    // Station save function
            EmbAJAXVarInt wifi_stn_id;                              // ID of WiFi station we are working with
            EmbAJAXServerFunction wifi_stn_btn;                     // Show station button
            EmbAJAXServerFunction wifi_stn_cnct;                    // Connect station button (on icon)

            EmbAJAXVarInt wifi_stn_count;                           // Max number of WiFi stations 
            EmbAJAXVarString<NET_MAX_SSID_LEN> wifi_stn_name;       // SSID of the WiFi station we are working with
            EmbAJAXVarBool wifi_stn_on;                             // Is the WiFi station we are working with connected
            EmbAJAXClientFunction LoadWifiList;                     // Client side function to load WiFi list

            EmbAJAXTextInput<NET_MAX_SSID_LEN> wifi_stn_ssid;       // Input box for the station SSID
            EmbAJAXTextInput<NET_MAX_PASSWORD_LEN> wifi_stn_pwd;    // Input box for the station password
            EmbAJAXCheckButton wifi_stn_dhcp;                       // Check box for the DHCP mode - checked is Dynamic
            EmbAJAXTextInput<16> wifi_stn_ip;                       // Input box for station IP address in static mode
            EmbAJAXTextInput<16> wifi_stn_snet;                     // Input box for station subnet in static mode
            EmbAJAXTextInput<16> wifi_stn_gtwy;                     // Input box for station gateway in static mode
            EmbAJAXTextInput<16> wifi_stn_dns1;                     // Input box for station DNS 1 in static mode
            EmbAJAXTextInput<16> wifi_stn_dns2;                     // Input box for station DNS 2 in static mode
            EmbAJAXMutableSpan wifi_stn_asip;                       // Span for assigned IP address

            EmbAJAXClientFunction ClearLoader;                      // Client side function clear the spinning loader

            // Internet check elements

            EmbAJAXCheckButton net_ck_enabled;                      // Check box for connectivity checker enabled
            EmbAJAXTextInput<4> net_ck_int;                         // Input box for connectivity checker interval (secs)
            EmbAJAXTextInput<NETCHECK_MAX_SERVICE_LEN> net_ck_url;  // Input box for connectivity checker URL
            EmbAJAXServerFunction net_ck_save;                      // Connectivity checker save button

            // DNS mode elements

            EmbAJAXCheckButton dns_enabled;                         // Check box for DNS enabled
            EmbAJAXCheckButton dns_mdns;                            // Check box for mDNS
            EmbAJAXTextInput<DNS_MAX_HOSTNAME_LEN> dns_name;        // Input box for hostname           // TODO - check this functionality
            EmbAJAXServerFunction dns_save;                         // DNS save button

            // Time and Location elemetns

            EmbAJAXServerFunction tlo_detect;                       // Detect location function
            EmbAJAXTextInput<TLO_IPINFO_MAX_TOKEN_LEN> tlo_token;   // Input box for IPInfo.io token
            EmbAJAXMutableSpan tlo_loc;                             // Span for reported location
            EmbAJAXMutableSpan tlo_tz;                              // Span for reported timezone
            EmbAJAXCheckButton tlo_ntp;                             // Check box for NTP enabled
            EmbAJAXServerFunction tlo_save;                         // Time and Locations save button

            // Array of page elements
            EmbAJAXBase* page_elements[WEB_PAGE_COMMON_ELEMENTS_COUNT + 42] = {

                WEB_PAGE_COMMON_ELEMENTS,       // Add the elements comment to every page

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
                &LoadWifiList,

                &wifi_stn_ssid,
                &wifi_stn_pwd,
                &wifi_stn_dhcp,
                &wifi_stn_ip,
                &wifi_stn_snet,
                &wifi_stn_gtwy,
                &wifi_stn_dns1,
                &wifi_stn_dns2,
                &wifi_stn_asip,

                &ClearLoader,

                &net_ck_enabled,
                &net_ck_int,
                &net_ck_url,
                &net_ck_save,

                &dns_enabled,
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

            /** Construct a new page object
             * @param ajaxHander        Pointer to the lamda function that handles ajax for this page
             * @param initHandler       Pointer to the lamda function that initializes this page */
            NetworkSettingsPage( void(*ajaxHandler)(), void(*initHandler)() ) : 
               
                wifi_mode_ap( "wifi_mode_ap", "" ),
                wifi_mode_stn( "wifi_mode_stn", "" ),
                wifi_mode_save( "wifi_mode_save" ),
            
                wifi_ap_save( "wifi_ap_save" ),
                wifi_ap_ssid( "wifi_ap_ssid" ),
                wifi_ap_pwd( "wifi_ap_pwd" ),
                wifi_ap_ip( "wifi_ap_ip" ),
                wifi_ap_snet( "wifi_ap_snet" ),
                wifi_ap_gtwy( "wifi_ap_gtwy" ),
                wifi_ap_ch( "wifi_ap_ch",wifi_chs),

                wifi_stn_save( "wifi_stn_save" ),
                wifi_stn_id( "wifi_stn_id" ),
                wifi_stn_btn( "wifi_stn_btn" ),
                wifi_stn_cnct( "wifi_stn_cnct" ),
                    
                wifi_stn_count( "wifi_stn_count",NET_MAX_STATIONS),
                wifi_stn_name( "wifi_stn_name", "" ),
                wifi_stn_on( "wifi_stn_on",false),
                LoadWifiList( "LoadWifiList" ),

                wifi_stn_ssid( "wifi_stn_ssid" ),
                wifi_stn_pwd( "wifi_stn_pwd" ),
                wifi_stn_dhcp( "wifi_stn_dhcp", "" ),
                wifi_stn_ip( "wifi_stn_ip" ),
                wifi_stn_snet( "wifi_stn_snet" ),
                wifi_stn_gtwy( "wifi_stn_gtwy" ),
                wifi_stn_dns1( "wifi_stn_dns1" ),
                wifi_stn_dns2( "wifi_stn_dns2" ),
                wifi_stn_asip( "wifi_stn_asip" ),

                ClearLoader( "ClearLoader" ),

                net_ck_enabled( "net_ck_enabled", "" ),
                net_ck_int( "net_ck_int" ),
                net_ck_url( "net_ck_url" ),
                net_ck_save( "net_ck_save" ),

                dns_enabled( "dns_enabled", "" ),
                dns_mdns( "dns_mdns", "" ),
                dns_name( "dns_name" ),
                dns_save( "dns_save" ),

                tlo_detect( "tlo_detect" ),
                tlo_token( "tlo_token" ),
                tlo_loc( "tlo_loc" ),
                tlo_tz( "tlo_tz" ),
                tlo_ntp( "tlo_ntp", "" ),
                tlo_save( "tlo_save" ),

                // Setup the EmbAJAX page base
                ajax( page_elements, "" )
                {
                    handler.URL = "/netsettings.html";
                    handler.ajaxHander = ajaxHandler;
                    handler.initHandler = initHandler;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;       // Instance of EmbAJAX for this page

            /** Function to initialize AJAX on this page */
            void ICACHE_FLASH_ATTR InitializeAjax();

            /** Function to handle AJAX requests for this page */
            void ICACHE_FLASH_ATTR HandleAjax();


        protected:

            /** Set the WiFi mode
             *  @param mode      Mode to set */
            void ICACHE_FLASH_ATTR SetWifiMode( WiFiMode mode );

            /** Load a specific WiFi station into the dialog
             *  @param id         ID of station */
            void ICACHE_FLASH_ATTR LoadWifiStation( uint id );

            /** Save a specific WiFi station from the dialog
             *  @param id         ID of station */
            void ICACHE_FLASH_ATTR SaveWifiStation( uint id );

            /** Connect a specific WiFi station
             *  @param id         ID of station */
            void ICACHE_FLASH_ATTR ConnectWifiStation( uint id );

            /** Save AP settings from page */
            void ICACHE_FLASH_ATTR SaveAP();

            /** Save connectivity checker settings from page */
            void ICACHE_FLASH_ATTR SaveNetCheck();

            /** Save DNS settings from page */
            void ICACHE_FLASH_ATTR SaveDNS();

            /** Save time and location settings from page */
            void ICACHE_FLASH_ATTR SaveTimeLocation();

            /** Load time and location settings to page */
            void ICACHE_FLASH_ATTR LoadTimeLocation();

            /** Call detect location using IPInfo.io */
            void ICACHE_FLASH_ATTR DetectLocation();

    };
    
    
    extern NetworkSettingsPage networksettingspage;          // Global instance of this page

#endif          // NETWORK_SETTINGS_PAGE_H