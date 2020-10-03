#include "LiteralManager.h"
#include "Device.h"
#include "ConfigManager.h"
#include "Logger.h"
#include "NetworkManager.h"
#include "WebManager.h"
#include "OTAUpdater.h"


void ICACHE_FLASH_ATTR setup() {

    device.begin();
    config.begin();
    logger.begin( network.getWiFiClient(), config.settings.logConfig );  
    network.begin( config.settings.networkConfig );
    website.begin();
    updater.begin( network.getWiFiClient(), config.settings.otaConfig );
    
    LOG(F("(Loop) Starting"));
    
}


void loop() {

  device.loop();
  network.handle();
  website.handle();
  updater.handle();
  logger.handle();

}