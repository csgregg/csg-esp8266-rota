#include "Logger.h"
#include "IOTDevice.h"
#include "ESPRemoteUpdater.h"
#include "ConfigManager.h"
#include "LiteralManager.h"
#include "NetworkManager.h"




void ICACHE_FLASH_ATTR elaborateBuildFlags() {

    LOG(device.getChipId(true));
    
    LOG(device_getBuildFlag(flag_PLATFORM, true));
    LOG(device_getBuildFlag(flag_BOARD,true));
    LOG(device_getBuildFlag(flag_FRAMEWORK,true));

    LOG(device_getBuildFlag(flag_DEVICE_NAME,true));
    LOG(device_getBuildFlag(flag_DEVICE_CODE,true));
    LOG(device_getBuildFlag(flag_BUILD_TAG,true));
    LOG(device_getBuildFlag(flag_BUILD_ENV,true));
    LOG(device_getBuildFlag(flag_BUILD_NO,true));
    LOG(device_getBuildFlag(flag_BUILD_TIMESTAMP,true));

    LOG(device_getBuildFlag(flag_UPDATER_REPO,true));
    LOG(device_getBuildFlag(flag_UPDATER_USER,true));
    LOG(device_getBuildFlag(flag_UPDATER_TOKEN,true));
    LOG(device_getBuildFlag(flag_UPDATER_SERVICE,true));
    LOG(device_getBuildFlag(flag_UPDATER_INTERVAL,true));
    LOG(device_getBuildFlag(flag_UPDATER_SKIP,true));

    LOG(device_getBuildFlag(flag_LOGGER_LEVEL,true));
    LOG(device_getBuildFlag(flag_LOGGER_AS_SERIAL,true));
    LOG(device_getBuildFlag(flag_LOGGER_AS_SERVICE,true));
    LOG(device_getBuildFlag(flag_LOGGER_SERVICE,true));
    LOG(device_getBuildFlag(flag_LOGGER_SERVICE_KEY,true));
    LOG(device_getBuildFlag(flag_LOGGER_GLOBAL_TAGS,true));
    LOG(device_getBuildFlag(flag_MONITOR_SPEED,true));
       
}


void ICACHE_FLASH_ATTR setup() {

    logger.begin( network.getWiFiClient(), device_getBuildFlag(flag_MONITOR_SPEED), device_getBuildFlag(flag_LOGGER_SERVICE), device_getBuildFlag(flag_LOGGER_SERVICE_KEY), device_getBuildFlag(flag_LOGGER_GLOBAL_TAGS) );    
    logger.setMode( device_getBuildFlag(flag_LOGGER_AS_SERIAL), false, loggingLevel(device_getBuildFlag(flag_LOGGER_LEVEL)) );

    config.Initialize();
    config.ResetToDefaults();

    network.begin( config.settings.networkConfig );

    logger.setMode( device_getBuildFlag(flag_LOGGER_AS_SERIAL), device_getBuildFlag(flag_LOGGER_AS_SERVICE), loggingLevel(device_getBuildFlag(flag_LOGGER_LEVEL)) );

    updater.setup( device_getBuildFlag(flag_UPDATER_SERVICE), device_getBuildFlag(flag_UPDATER_REPO), device_getBuildFlag(flag_UPDATER_USER), device_getBuildFlag(flag_UPDATER_TOKEN), device_getBuildFlag(flag_DEVICE_CODE), device_getBuildFlag(flag_BUILD_TAG), device_getBuildFlag(flag_UPDATER_INTERVAL), device_getBuildFlag(flag_UPDATER_SKIP) );
    updater.begin( network.getWiFiClient() );
    
    elaborateBuildFlags();
    LOG(F("(Loop) Starting"));

    
}


void loop() {

  updater.handle();
  network.handle();

  

}