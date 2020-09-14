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


*/


#include "SystemPage.h"
#include "Device.h"
#include "Logger.h"
#include "ConfigManager.h"

// TODO: Flashstrings


void SystemPage::initializeAjax(){

    LOG_HIGH("(Page) Index - Initialize AJAX");

    LogSettings logger = config.settings.logConfig;

    char buffer[8];

    log_srl.setChecked( logger.serialMode );
    log_baud.setValue( itoa(logger.serialBaud,buffer,10) );
    log_ser.setChecked( logger.serviceMode );
    log_url.setValue( logger.serviceURL );
    log_key.setValue( logger.serviceKey );
    log_tick.setChecked( logger.tickMode );
    log_tick_int.setValue( itoa(logger.tickInterval,buffer,10) );
    log_tags.setValue( logger.globalTags );
    log_level.selectOption( logger.level );

}

void SystemPage::handleAjax(){

    LOG_HIGH("(Page) System - Handle AJAX");

    if( website.AjaxID == "btn_restart" ) device.restart();

    if( website.AjaxID == "btn_rst_net" ){
        config.settings.networkConfig.setDefaults();
        config.Save();
    }

    if( website.AjaxID == "btn_rst_log" ){
        config.settings.logConfig.setDefaults();
        config.Save();
    }

    if( website.AjaxID == "btn_rst_all" ){
        config.ResetToDefaults();
    }

    if( website.AjaxID == "log_save" ) saveLogConfig();

}


void SystemPage::saveLogConfig() {
    
    LogSettings log;
        
    log.serialMode = log_srl.isChecked();   
    log.serialBaud = atoi(log_baud.value());
    log.serviceMode = log_ser.isChecked();
    strncpy(log.serviceURL,log_url.value(),MAX_SERVICE_LEN);
    strncpy(log.serviceKey,log_key.value(),MAX_KEY_LEN);
    log.tickMode = log_tick.isChecked();
    log.tickInterval = atoi(log_tick_int.value());
    strncpy(log.globalTags,log_tags.value(),MAX_GLOBAL_TAG_LEN);
    log.level = (logLevel)atoi(log_level.value());

    config.settings.logConfig = log;
    config.Save();

    logger.begin(config.settings.logConfig);

}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SYSTEMPAGE)

    SystemPage systempage(
        []() { 
            systempage.ajax.handleRequest( 
                []() {
                    systempage.handleAjax();
                }
            ); 
        },
        []() { 
            systempage.initializeAjax();
        }
    );

#endif