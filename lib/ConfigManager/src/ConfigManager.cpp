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

Manages saving a configuration set to the EEPROM section of the flash (First
sector before file system). Settings are read and write on block and accessed
through the settings member.

*/

#include <EEPROM.h>


#include "ConfigManager.h"
#include "Logger.h"
#include "IOTDevice.h"


// Public:

// Consructor
ConfigManager::ConfigManager() {
    _IsInitialized = false;             // Don't do anything until flash is initialized   
}


// Initialize the flash
void ICACHE_FLASH_ATTR ConfigManager::Initialize( const bool forceInit ) {

    // Start it up
    if( !_IsInitialized ) {

        EEPROM.begin(SPI_FLASH_SEC_SIZE);
		_IsInitialized = true;

        LOG(F("(Config) Starting Configuation Manager"));
    }

    // Check to see if it has been used before and has the right marker in place to confim
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

        // Flash is already set up and ready to use
        LOG(F("(Config) Flash ready"));

        Read();         // Get the settings for the first time
    }

}


// Saves the default settings to flash
void ICACHE_FLASH_ATTR ConfigManager::ResetToDefaults() {

    LOG(F("(Config) Reset to defaults"));

    if( !_IsInitialized ) {
        LOG_CRITICAL(F("(Config) Flash not yet initialized - cannot save"));
        return;
    }

    // Default settings

    settings.networkConfig.setWiFiDefaults();


    // Save to flash
    Save(true);
}


// Read settings from flash 
void ICACHE_FLASH_ATTR ConfigManager::Read() {

    LOG(F("(Config) Reading settings"));

    // Don't do anything unless ready to use
    if( !_IsInitialized ) {
        LOG_CRITICAL(F("(Config) Flash not yet initialized - cannot read"));
        return;
    }

    // Get the settings
	settings = EEPROM.get(markerDataSize, settings);
}


void ICACHE_FLASH_ATTR ConfigManager::Save( const bool force ) {

    LOG(F("(Config) Saving settings"));

    // Don't do anything unless ready to use
    if( !_IsInitialized ) {
        LOG_CRITICAL(F("(Config) Flash not yet initialized - cannot save"));
        return;
    }

    // Check to see what is there first and only save if differnet 

    if( !force ) {

        deviceSettings readset = EEPROM.get(markerDataSize, readset);

        // TODO - is this necessary?
        if( readset == settings ) {

            LOG(F("(Config) Settings unchanged - not saving"));

            return;
        }
    }

    // Different so save

    EEPROM.put(markerDataSize, settings);
	EEPROM.commit();

    LOG(F("(Config) New sttings saved"));

}


// Protected:


// Look for the marker that was placed when the flash was set up
bool ICACHE_FLASH_ATTR ConfigManager::CheckMarker() {

	startMarker markerData = EEPROM.get(0, markerData);

    logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
    PGM_P format = PSTR("(Config) Marker from flash: %s");
    logger.printf( format, markerData.marker );
    
    // Return if it is there
    return (strcmp(markerData.marker, CONFIG_START_MARKER) == 0);
}


// Write the marker that shows that the flash is set up
void ICACHE_FLASH_ATTR ConfigManager::WriteMarker() {

    startMarker markerData;

    logger.setTypeTag(LOG_DETAIL, TAG_STATUS);
    PGM_P format = PSTR("(Config) Writing marker: %s");
    logger.printf( format, markerData.marker );

    strncpy(markerData.marker, CONFIG_START_MARKER, sizeof(CONFIG_START_MARKER));
	markerData = EEPROM.put(0, markerData);
}


// Erase the content of the flash to 0
void ICACHE_FLASH_ATTR ConfigManager::EraseFlash() {

    LOG(F("(Config) Erasing flash"));

	for (uint16_t i = 0 ; i < SPI_FLASH_SEC_SIZE ; i++) {
		EEPROM.write(i, 0);
		yield();
	}

    LOG(F("(Config) Flash erased"));

}


// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_CONFIG_SETTINGS)
    ConfigManager config;
#endif