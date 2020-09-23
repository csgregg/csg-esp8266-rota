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

Server-side functions of system.html

*/


#ifndef SYSTEM_PAGE_H

    #define SYSTEM_PAGE_H


    #include "WebManager.h"
    #include "Logger.h"


    class SystemPage {
        public:

            const char* URL;
            void (*handler)();
            void (*init)();

            LogSettings loggerSettings;

            EmbAJAXServerFunction btn_restart;
            EmbAJAXServerFunction btn_rst_net;
            EmbAJAXServerFunction btn_rst_all;

            EmbAJAXCheckButton log_srl;
            EmbAJAXTextInput<7> log_baud;
            EmbAJAXCheckButton log_ser;
            EmbAJAXTextInput<MAX_SERVICE_LEN> log_url;
            EmbAJAXTextInput<MAX_KEY_LEN> log_key;
            EmbAJAXCheckButton log_tick;
            EmbAJAXTextInput<4> log_tick_int;
            EmbAJAXTextInput<MAX_GLOBAL_TAG_LEN> log_tags;
            EmbAJAXOptionSelect<5> log_level;
            const char* logLevels[5] = {"0","1","2","3","4"};
            EmbAJAXServerFunction log_save;

            EmbAJAXBase* page_elements[14] = {
      
                &net_status,

                &btn_restart,
                &btn_rst_net,
                &btn_rst_all,

                &log_srl,
                &log_baud,
                &log_ser,
                &log_url,
                &log_key,
                &log_tick,
                &log_tick_int,
                &log_tags,
                &log_level,
                &log_save

            };

           SystemPage( void(*phandler)(), void(*pinit)() ) : 
              
                btn_restart("btn_restart"),
                btn_rst_net("btn_rst_net"),
                btn_rst_all("btn_rst_all"),

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

                ajax(page_elements, "")
                {
                    URL = "/system.html";
                    handler = phandler;
                    init = pinit;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

            void ICACHE_FLASH_ATTR handleAjax();

            void ICACHE_FLASH_ATTR initializeAjax();

            void ICACHE_FLASH_ATTR saveLogConfig();
    };
    
    extern SystemPage systempage;

#endif