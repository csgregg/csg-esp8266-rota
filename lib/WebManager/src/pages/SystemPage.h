/**
 * @file        SystemPage.h
 * @author      Chris Gregg
 * 
 * @brief       Server-side functions of system.html
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

/* MIT License

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
SOFTWARE. */


#ifndef SYSTEM_PAGE_H

    #define SYSTEM_PAGE_H

    // Global Libraries
    #include <Arduino.h>

    // Project Libraries
    #include "WebManager.h"
    #include "Logger.h"
    #include "OTAUpdater.h"


    /** @class SystemPage
     *  @brief Server-side functions for system.html page */
    class SystemPage {

        public:

            PageHandler handler;                    // Handler for this page

            LoggerSettings loggerSettings;          // Holder for logger settings

            // Restart and reset elements

            EmbAJAXServerFunction btn_restart;      // Restart device button
            EmbAJAXServerFunction btn_rst_net;      // Reset network settings button
            EmbAJAXServerFunction btn_rst_all;      // Reset all settings button
            EmbAJAXServerFunction btn_rst_log;      // Reset logger settings button
            EmbAJAXServerFunction btn_rst_ota;      // Reset OTA updater settings button
            EmbAJAXServerFunction btn_rst_tlo;      // Reset Time and Location settings button

            // Logger elements

            EmbAJAXCheckButton log_srl;                             // Check box for serial mode
            EmbAJAXTextInput<7> log_baud;                           // Input box for serial baud
            EmbAJAXCheckButton log_ser;                             // Check box for service mode
            EmbAJAXTextInput<LOG_MAX_SERVICE_LEN> log_url;          // Input box for Loggly URL
            EmbAJAXTextInput<LOG_MAX_KEY_LEN> log_key;              // Input box for Loggly token
            EmbAJAXCheckButton log_tick;                            // Check box for tick service
            EmbAJAXTextInput<4> log_tick_int;                       // Input box for tick interval
            EmbAJAXTextInput<LOG_MAX_GLOBAL_TAG_LEN> log_tags;      // Input box for global tags
            EmbAJAXOptionSelect<5> log_level;                       // Dropdown for logging level
            const char* logLevels[5] = {"0","1","2","3","4"};       // Options for logging level dropdown
            EmbAJAXServerFunction log_save;                         // Save logger settings button

            /// OTA Updater elements

            EmbAJAXCheckButton ota_mode;                            // Check box for mode
            EmbAJAXTextInput<OTA_MAX_SERVICE_LEN> ota_url;          // Input box for service URL
            EmbAJAXTextInput<OTA_MAX_USER_LEN> ota_user;            // Input box for GitHub user
            EmbAJAXTextInput<OTA_MAX_REPO_LEN> ota_repo;            // Input box for GitHub repo
            EmbAJAXTextInput<OTA_MAX_TOKEN_LEN> ota_key;            // Input box for GitHub token
            EmbAJAXCheckButton ota_skip;                            // Check box for update skip
            EmbAJAXTextInput<4> ota_ck_int;                         // Input box for check interval
            EmbAJAXServerFunction ota_save;                         // Save udpater settings button

            // Array of page elements
            EmbAJAXBase* page_elements[WEB_PAGE_COMMON_ELEMENTS_COUNT + 24] = {
      
                WEB_PAGE_COMMON_ELEMENTS,       // Add the elements comment to every page

                &btn_restart,
                &btn_rst_net,
                &btn_rst_all,
                &btn_rst_log,
                &btn_rst_ota,
                &btn_rst_tlo,

                &log_srl,
                &log_baud,
                &log_ser,
                &log_url,
                &log_key,
                &log_tick,
                &log_tick_int,
                &log_tags,
                &log_level,
                &log_save,

                &ota_mode,
                &ota_url,
                &ota_user,
                &ota_repo,
                &ota_key,
                &ota_skip,
                &ota_ck_int,
                &ota_save,

            };

            /** Construct a new page object
             * @param ajaxHander        Pointer to the lamda function that handles ajax for this page
             * @param initHandler       Pointer to the lamda function that initializes this page */
           SystemPage( void(*ajaxHandler)(), void(*initHandler)() ) : 
              
                btn_restart("btn_restart"),
                btn_rst_net("btn_rst_net"),
                btn_rst_all("btn_rst_all"),
                btn_rst_log("btn_rst_log"),
                btn_rst_ota("btn_rst_ota"),
                btn_rst_tlo("btn_rst_tlo"),

                log_srl("log_srl",""),
                log_baud("log_baud"),
                log_ser("log_ser",""),
                log_url("log_url"),
                log_key("log_key"),
                log_tick("log_tick",""),
                log_tick_int("log_tick_int"),
                log_tags("log_tags"),
                log_level("log_level",logLevels),
                log_save("log_save"),

                ota_mode("ota_mode",""),
                ota_url("ota_url"),
                ota_user("ota_user"),
                ota_repo("ota_repo"),
                ota_key("ota_key"),
                ota_skip("ota_skip",""),
                ota_ck_int("ota_ck_int"),
                ota_save("ota_save"),

                // Setup the EmbAJAX page base
                ajax(page_elements, "")
                {
                    handler.URL = "/system.html";
                    handler.ajaxHander = ajaxHandler;
                    handler.initHandler = initHandler;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;       // Instance of EmbAJAX for this page

            /** Function to initialize AJAX on this page */
            void ICACHE_FLASH_ATTR InitializeAjax();

            /** Function to handle AJAX requests for this page */
            void ICACHE_FLASH_ATTR HandleAjax();


        protected:

            /** Save the logger settings */
            void ICACHE_FLASH_ATTR SaveLoggerSettings();

            /** Save OTA Updater settings */
            void ICACHE_FLASH_ATTR SaveUpdaterSettings();

    };
    

    extern SystemPage systempage;          // Global instance of this page

#endif              // SYSTEM_PAGE_H