#include "Device.h"
#include "ConfigManager.h"
#include "Logger.h"
#include "NetworkManager.h"
#include "WebManager.h"
#include "OTAUpdater.h"
#include "TimeLocation.h"


void ICACHE_FLASH_ATTR setup() {

    // Services started in the proper order
    device.begin();
    config.begin();
    logger.begin( network.getWiFiClient(), config.settings.logSettings );  
    network.begin( config.settings.networkSettings );
    timelocation.begin( network.getWiFiClient(), config.settings.timelocsettings );
    website.begin( config.settings.networkSettings.dnsSettings.hostname );
    updater.begin( network.getWiFiClient(), config.settings.otaSettings );
    
    LOG(PSTR("(Loop) Starting"));        // TODO - Check all LOG levels for all instances
    
}


void loop() {

    // Handle each service set
    device.handle();
    network.handle();
    website.handle();
    updater.handle();
    logger.handle();
    timelocation.handle();

}