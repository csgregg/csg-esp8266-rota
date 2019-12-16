/* COnfig Manager Library

MIT License

Copyright (c) 2019 Chris Gregg

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

#include <EEPROM.h>


#include "ConfigManager.h"
#include "Logger.h"






// Public:

// Consructor
ConfigManager::ConfigManager() {
    _IsInitialized = false;

    
}


void ICACHE_FLASH_ATTR ConfigManager::Initialize( bool forceInit ) {

    if( !_IsInitialized ) {

        EEPROM.begin(SPI_FLASH_SEC_SIZE);
		_IsInitialized = true;

        LOG(F("(Config) Starting Configuation Manager"));
    }

    if( !CheckMarker() || forceInit ) {

        LOG(F("(Config) Initializing flash"));

        // Erase flash
        EraseFlash();

        // Initialize flag
        WriteMarker();

        // Reset to defauls
        ResetToDefaults();

    }
    else {

        LOG(F("(Config) Flash ready"));

        Read();
    }

}


void ICACHE_FLASH_ATTR ConfigManager::ResetToDefaults() {

    LOG(F("(Config) Reset to defaults"));

    // Default settings
    Settings.wifiMode = WIFI_OFF;

    for( int i = 0; i<MAX_SSIDS; i++ ) {
        strcpy(Settings.clientSetting[i].clientSSID, "SSID");
        strcpy(Settings.clientSetting[i].clientPwd, "PASS");
        Settings.clientSetting[i].clientDHCPMode = DHCP;
        Settings.clientSetting[i].clientStaticIP.fromString("123.123.123.123");
    }

    strcpy(Settings.apSSID, "SSID");
    strcpy(Settings.apPwd, "PWD");
    Settings.apChannel = 13;

    // Save to flash
    Save(true);
}


void ICACHE_FLASH_ATTR ConfigManager::Read() {

    LOG(F("(Config) Reading settings"));

	Settings = EEPROM.get(markerDataSize, Settings);

}


void ICACHE_FLASH_ATTR ConfigManager::Save( bool force ) {

    LOG(F("(Config) Saving settings"));

    // Check to see what is there first and only save if differnet 

    if( !force ) {

        deviceSettings readset = EEPROM.get(markerDataSize, readset);

        if( readset == Settings ) {

            LOG(F("(Config) Settings unchanged - not saving"));

            return;
        }
    }

    // Different so save

    EEPROM.put(markerDataSize, Settings);
	EEPROM.commit();

    LOG(F("(Config) New sttings saved"));

}


// Protected:


bool ICACHE_FLASH_ATTR ConfigManager::CheckMarker() {

	startMarker markerData = EEPROM.get(0, markerData);

    logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
    PGM_P format = PSTR("(Config) Marker from flash: %s");
    logger.printf( format, markerData.marker );
    
    return (strcmp(markerData.marker, CONFIG_START_MARKER) == 0);

}


void ICACHE_FLASH_ATTR ConfigManager::WriteMarker() {

    startMarker markerData;

    logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
    PGM_P format = PSTR("(Config) Writing marker: %s");
    logger.printf( format, markerData.marker );

    strncpy(markerData.marker, CONFIG_START_MARKER, sizeof(CONFIG_START_MARKER));
	markerData = EEPROM.put(0, markerData);
}


void ICACHE_FLASH_ATTR ConfigManager::EraseFlash() {

    LOG(F("(Config) Erasing flash"));

	for (uint16_t i = 0 ; i < SPI_FLASH_SEC_SIZE ; i++) {
		EEPROM.write(i, 0);
		yield();
	}

    LOG(F("\n(Config) Flash erased"));

}


// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_CONFIG_SETTINGS)
    ConfigManager config;
#endif