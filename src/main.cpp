#include "LiteralManager.h"
#include "Device.h"
#include "ConfigManager.h"
#include "Logger.h"
#include "NetworkManager.h"
#include "WebManager.h"
#include "OTAUpdater.h"


void ICACHE_FLASH_ATTR elaborateBuildFlags() {

    LOG_FLAG(flag_BOARD);
    LOG_FLAG(flag_DEVICE_NAME);
    LOG_FLAG(flag_DEVICE_CODE);
    LOG_FLAG(flag_BUILD_TAG);
    LOG_FLAG(flag_BUILD_ENV);
    LOG_FLAG(flag_BUILD_NO);
    LOG_FLAG(flag_BUILD_TIMESTAMP);

    LOG_FLAG(flag_UPDATER_REPO);
    LOG_FLAG(flag_UPDATER_USER);
    LOG_FLAG(flag_UPDATER_TOKEN);
    LOG_FLAG(flag_UPDATER_SERVICE);
    LOG_FLAG(flag_UPDATER_INTERVAL);
    LOG_FLAG(flag_UPDATER_SKIP);

    LOG_FLAG(flag_LOGGER_LEVEL);
    LOG_FLAG(flag_LOGGER_AS_SERIAL);
    LOG_FLAG(flag_LOGGER_AS_SERVICE);
    LOG_FLAG(flag_LOGGER_TICKER);
    LOG_FLAG(flag_LOGGER_TICK_INTERVAL);
    LOG_FLAG(flag_LOGGER_SERVICE);
    LOG_FLAG(flag_LOGGER_SERVICE_KEY);
    LOG_FLAG(flag_LOGGER_GLOBAL_TAGS);
    LOG_FLAG(flag_MONITOR_SPEED);

    LOG_FLAG(flag_NET_CHECKER);
    LOG_FLAG(flag_NET_CHECK_SERVICE);
    LOG_FLAG(flag_NET_CHECK_INTERVAL);
       
}


void ICACHE_FLASH_ATTR setup() {

    device.begin();
    config.begin();
    logger.begin( network.getWiFiClient(), config.settings.logConfig );  
    network.begin( config.settings.networkConfig );
    website.begin();
    updater.begin( network.getWiFiClient(), config.settings.otaConfig );
    
    // elaborateBuildFlags();
    
    LOG(F("(Loop) Starting"));
    
}


void loop() {

  network.handle();
  website.handle();
  updater.handle();
  logger.handle();

}