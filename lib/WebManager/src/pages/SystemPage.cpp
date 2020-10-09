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


#include "SystemPage.h"
#include "Device.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "NetworkManager.h"
#include "OTAUpdater.h"


void ICACHE_FLASH_ATTR SystemPage::initializeAjax(){

    LOG_HIGH(PSTR("(Page) System - Initialize AJAX"));

    LogSettings logger = config.settings.logConfig;
    OTASettings ota = config.settings.otaConfig;

    static char buffer[8];

    log_srl.setChecked( logger.serialMode );
    log_baud.setValue( itoa(logger.serialBaud,buffer,10) );
    log_ser.setChecked( logger.serviceMode );
    log_url.setValue( logger.serviceURL );
    log_key.setValue( logger.serviceKey );
    log_tick.setChecked( logger.tickMode );
    log_tick_int.setValue( itoa(logger.tickInterval,buffer,10) );
    log_tags.setValue( logger.globalTags );
    log_level.selectOption( logger.level );
    log_save.setEnabled(false);

    ota_mode.setChecked( ota.mode );
    ota_url.setValue( ota.service );
    ota_user.setValue( ota.user );
    ota_repo.setValue( ota.repo );
    ota_key.setValue( ota.token );
    ota_skip.setChecked( ota.skipUpdates );
    ota_ck_int.setValue( itoa(ota.interval,buffer,10) );
    ota_save.setEnabled(false);

}

void ICACHE_FLASH_ATTR SystemPage::handleAjax(){

    LOG_HIGH(PSTR("(Page) System - Handle AJAX"));

    if( website.AjaxID == F("btn_restart") ) device.restart();

    if( website.AjaxID == F("btn_rst_net") ){
        config.settings.networkConfig.setDefaults();
        config.Save();
    }

    if( website.AjaxID == F("btn_rst_log") ){
        config.settings.logConfig.setDefaults();
        config.Save();
    }

    if( website.AjaxID == F("btn_rst_all") ){
        config.setDefaults();
        config.Save();
    }

    if( website.AjaxID == F("btn_rst_ota") ){
        config.settings.otaConfig.setDefaults();
        config.Save();
    }

    if( website.AjaxID == F("log_save") ) saveLogConfig();

    if( website.AjaxID == F("ota_save") ) saveOTAConfig();

}


void ICACHE_FLASH_ATTR SystemPage::saveLogConfig() {
    
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


void ICACHE_FLASH_ATTR SystemPage::saveOTAConfig() {
    
    OTASettings ota;

    ota.mode = ota_mode.isChecked();
    strncpy(ota.service,ota_url.value(),OTA_MAX_SERVICE_LEN);
    strncpy(ota.user,ota_user.value(),OTA_MAX_USER_LEN);
    strncpy(ota.repo,ota_repo.value(),OTA_MAX_REPO_LEN);
    strncpy(ota.token,ota_key.value(),OTA_MAX_TOKEN_LEN);
    ota.skipUpdates = ota_skip.isChecked();
    ota.interval = atoi(ota_ck_int.value());

    config.settings.otaConfig = ota;
    config.Save();

    updater.begin(config.settings.otaConfig);           

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