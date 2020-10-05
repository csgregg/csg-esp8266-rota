#include "LiteralManager.h"
#include "Device.h"
#include "ConfigManager.h"
#include "Logger.h"
#include "NetworkManager.h"
#include "WebManager.h"
#include "OTAUpdater.h"


void ICACHE_FLASH_ATTR setup() {

    // Services started in the proper order
    device.begin();
    config.begin();
    logger.begin( network.getWiFiClient(), config.settings.logConfig );  
    network.begin( config.settings.networkConfig );
    website.begin( config.settings.networkConfig.dnsSettings.hostname );
    updater.begin( network.getWiFiClient(), config.settings.otaConfig );
    
    LOG(PSTR("(Loop) Starting"));        // TODO - Check all LOG levels for all instances
    
}


void loop() {

    // Handle each service set
    device.handle();
    network.handle();
    website.handle();
    updater.handle();
    logger.handle();

}