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
#include "IOTDevice.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "NetworkManager.h"
#include "OTAUpdater.h"


void ICACHE_FLASH_ATTR SystemPage::initializeAjax(){

    LOG_HIGH(PSTR("(Page) System - Initialize AJAX"));

    LoggerSettings log = config.settings.logSettings;
    OTASettings ota = config.settings.otaSettings;

    static char buffer[8];

    log_srl.setChecked( log.serialModeOn );
    log_baud.setValue( itoa(log.serialBaud,buffer,10) );
    log_ser.setChecked( log.serviceModeOn );
    log_url.setValue( log.serviceURL );
    log_key.setValue( log.serviceKey );
    log_tick.setChecked( log.tickModeOn );
    log_tick_int.setValue( itoa(log.tickInterval,buffer,10) );
    log_tags.setValue( log.globalTags );
    log_level.selectOption( log.level );
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
        config.settings.networkSettings.SetDefaults();
        config.Save();
        return;
    }

    if( website.AjaxID == F("btn_rst_log") ){
        config.settings.logSettings.SetDefaults();
        config.Save();
        logger.Restart(config.settings.logSettings);
        return;
    }

    if( website.AjaxID == F("btn_rst_all") ){
        config.SetDefaults();
        config.Save();
        return;
    }

    if( website.AjaxID == F("btn_rst_ota") ){
        config.settings.otaSettings.setDefaults();
        config.Save();
        updater.begin(config.settings.otaSettings);
        return;
    }

    if( website.AjaxID == F("btn_rst_tlo") ) {
        config.settings.timelocsettings.setDefaults();
        config.Save();
        timelocation.begin(config.settings.timelocsettings);
        return;
    }

    if( website.AjaxID == F("log_save") ) {
        saveLogConfig();
        return;
    }

    if( website.AjaxID == F("ota_save") ) {
        saveOTAConfig();
        return;
    }

}


void ICACHE_FLASH_ATTR SystemPage::saveLogConfig() {
    
    LoggerSettings log;
        
    log.serialModeOn = log_srl.isChecked();   
    log.serialBaud = atoi(log_baud.value());
    log.serviceModeOn = log_ser.isChecked();
    strncpy(log.serviceURL,log_url.value(),LOG_MAX_SERVICE_LEN);
    strncpy(log.serviceKey,log_key.value(),LOG_MAX_KEY_LEN);
    log.tickModeOn = log_tick.isChecked();
    log.tickInterval = atoi(log_tick_int.value());
    strncpy(log.globalTags,log_tags.value(),LOG_MAX_GLOBAL_TAG_LEN);
    log.level = LogLevel(atoi(log_level.value()));

    config.settings.logSettings = log;
    config.Save();

    logger.Restart(config.settings.logSettings);

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

    config.settings.otaSettings = ota;
    config.Save();

    updater.begin(config.settings.otaSettings);           

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