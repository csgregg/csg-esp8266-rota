/**
 * @file        SystemPage.cpp
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


// Project Libraries
#include "SystemPage.h"
#include "IOTDevice.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "NetworkManager.h"
#include "OTAUpdater.h"


////////////////////////////////////////////
//// Sytsem Page Class

// Public:

// Function to initialize AJAX on this page
void ICACHE_FLASH_ATTR SystemPage::InitializeAjax(){

    LOG_HIGH( PSTR("(Page) System - Initialize AJAX") );

    LoggerSettings log = config.settings.loggerSettings;
    OTAUpdaterSettings ota = config.settings.otaUpdaterSettings;

    static char buffer[8];

    log_srl.setChecked( log.serialModeOn );
    log_baud.setValue( itoa( log.serialBaud, buffer, 10 ) );
    log_ser.setChecked( log.serviceModeOn );
    log_url.setValue( log.serviceURL );
    log_key.setValue( log.serviceKey );
    log_tick.setChecked( log.tickModeOn );
    log_tick_int.setValue( itoa( log.tickInterval,buffer, 10 ) );
    log_tags.setValue( log.globalTags );
    log_level.selectOption( log.level );
    log_save.setEnabled( false) ;

    ota_mode.setChecked( ota.enabled );
    ota_url.setValue( ota.service );
    ota_user.setValue( ota.user );
    ota_repo.setValue( ota.repo );
    ota_key.setValue( ota.token );
    ota_skip.setChecked( ota.skipUpdates );
    ota_ck_int.setValue( itoa( ota.interval, buffer, 10 ) );
    ota_save.setEnabled( false );

}


// Function to handle AJAX requests for this page
void ICACHE_FLASH_ATTR SystemPage::HandleAjax(){

    LOG_HIGH( PSTR("(Page) System - Handle AJAX") );

    // Restart device
    if( website.AjaxID == F("btn_restart") ) device.restart();

    // Reset network settigns
    if( website.AjaxID == F("btn_rst_net") ){
        config.settings.networkSettings.SetDefaults();
        config.Save();
        return;
    }

    // Reset logger settings
    if( website.AjaxID == F("btn_rst_log") ){
        config.settings.loggerSettings.SetDefaults();
        config.Save();
        logger.Restart( config.settings.loggerSettings );
        return;
    }

    // Reset all settings
    if( website.AjaxID == F("btn_rst_all") ){
        config.SetDefaults();
        config.Save();
        return;
    }

    // Reset OTA Update settings
    if( website.AjaxID == F("btn_rst_ota") ){
        config.settings.otaUpdaterSettings.SetDefaults();
        config.Save();
        updater.Restart( config.settings.otaUpdaterSettings );
        return;
    }

    // Reset time and location settings
    if( website.AjaxID == F("btn_rst_tlo") ) {
        config.settings.timelocSettings.SetDefaults();
        config.Save();
        timelocation.Restart( config.settings.timelocSettings );
        return;
    }

    // Save logger settings
    if( website.AjaxID == F("log_save") ) {
        SaveLoggerSettings();
        return;
    }

    // Save OTA update settings
    if( website.AjaxID == F("ota_save") ) {
        SaveUpdaterSettings();
        return;
    }

}


// Save the logger settings
void ICACHE_FLASH_ATTR SystemPage::SaveLoggerSettings() {
    
    LoggerSettings log;
        
    log.serialModeOn = log_srl.isChecked();   
    log.serialBaud = atoi( log_baud.value() );
    log.serviceModeOn = log_ser.isChecked();
    strncpy( log.serviceURL,log_url.value(), LOG_MAX_SERVICE_LEN );
    strncpy( log.serviceKey,log_key.value(), LOG_MAX_KEY_LEN );
    log.tickModeOn = log_tick.isChecked();
    log.tickInterval = atoi( log_tick_int.value() );
    strncpy( log.globalTags,log_tags.value(), LOG_MAX_GLOBAL_TAG_LEN );
    log.level = LogLevel(atoi( log_level.value() ));

    config.settings.loggerSettings = log;
    config.Save();
    logger.Restart( config.settings.loggerSettings );

}


// Save OTA Updater settings
void ICACHE_FLASH_ATTR SystemPage::SaveUpdaterSettings() {
    
    OTAUpdaterSettings ota;

    ota.enabled = ota_mode.isChecked();
    strncpy( ota.service, ota_url.value(), OTA_MAX_SERVICE_LEN );
    strncpy( ota.user, ota_user.value(), OTA_MAX_USER_LEN );
    strncpy( ota.repo, ota_repo.value(), OTA_MAX_REPO_LEN );
    strncpy( ota.token, ota_key.value(), OTA_MAX_TOKEN_LEN );
    ota.skipUpdates = ota_skip.isChecked();
    ota.interval = atoi( ota_ck_int.value() );

    config.settings.otaUpdaterSettings = ota;
    config.Save();

    updater.Restart( config.settings.otaUpdaterSettings );           

}


// Create instance of page class and wrap methods for EmbAJAX
SystemPage systempage(
    []() { 
        systempage.ajax.handleRequest( 
            []() {
                systempage.HandleAjax();
            }
        ); 
    },
    []() { 
        systempage.InitializeAjax();
    }
);