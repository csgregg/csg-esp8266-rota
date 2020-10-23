/**
 * @file        ConfigManager.cpp
 * @author      Chris Gregg
 * 
 * @brief       Manages saving a configuration set to the EEPROM section of the flash (First
 *              sector before file system). Settings are read and write on block and accessed
 *              through the settings member.
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


// Global Libraries
#include <EEPROM.h>

// Project Libraries
#include "ConfigManager.h"
#include "Logger.h"
#include "IOTDevice.h"




////////////////////////////////////////////
//// Config Manager Class

// Public:

// Resets all the settings to the default values */
void ICACHE_FLASH_ATTR DeviceSettings::SetDefaults() {
    networkSettings.SetDefaults();
    loggerSettings.SetDefaults();
    otaUpdaterSettings.SetDefaults();
    timelocSettings.SetDefaults();
}


////////////////////////////////////////////
//// Config Manager Class

// Public:

// Start the configuraiton manager
void ICACHE_FLASH_ATTR ConfigManager::Begin( const bool forceInit )
{

    // Start it up
    if( !_isInitialized ) {

        EEPROM.begin( SPI_FLASH_SEC_SIZE );
		_isInitialized = true;

        LOG( PSTR("(Config) Starting Configuation Manager") );

    }

    // Check to see if it has been used before and has the right marker in place to confim
    if( !CheckMarker() || forceInit ) {

        LOG( PSTR("(Config) Initializing flash") );

        // Erase flash
        EraseFlash();

        // Initialize flag
        WriteMarker();

        // Reset to defauls
        SetDefaults();
        Save();

    }
    else Read();         // Get the settings for the first time

}


// Saves the default settings to flash
void ICACHE_FLASH_ATTR ConfigManager::SetDefaults()
{

    LOG( PSTR("(Config) Reset to defaults") );

    settings.SetDefaults();

    // Save to flash
    Save( true );
}


// Read settings from flash 
void ICACHE_FLASH_ATTR ConfigManager::Read()
{

    LOG_HIGH( PSTR("(Config) Reading settings") );

    // Don't do anything unless ready to use
    if( !_isInitialized ) {
        LOG_CRITICAL( PSTR("(Config) Flash not yet initialized - cannot read") );
        return;
    }

    // Get the settings
	settings = EEPROM.get( sizeof(ConfigManager::startMarker), settings );
}


void ICACHE_FLASH_ATTR ConfigManager::Save( const bool force )
{

    LOG(PSTR("(Config) Saving settings"));

    // Don't do anything unless ready to use
    if( !_isInitialized ) {
        LOG_CRITICAL( PSTR("(Config) Flash not yet initialized - cannot save") );
        return;
    }

    if( !force ) {

        DeviceSettings readset = EEPROM.get( sizeof(ConfigManager::startMarker), readset );

        // Check to see what is there first and only save if different 
        if( readset == settings ) {

            LOG_HIGH( PSTR("(Config) Settings unchanged - not saving") );

            return;
        }
    }

    // Different so save

    EEPROM.put( sizeof(ConfigManager::startMarker), settings );
	EEPROM.commit();

    LOG( PSTR("(Config) New sttings saved") );

}


// Protected:

// Look for the marker that was placed when the flash was set up
bool ICACHE_FLASH_ATTR ConfigManager::CheckMarker()
{

	startMarker markerData = EEPROM.get( 0, markerData );
    
    // Return if it is there
    return ( strcmp( markerData.marker, CONFIG_START_MARKER ) == 0 );
}


// Write the marker that shows that the flash is set up
void ICACHE_FLASH_ATTR ConfigManager::WriteMarker()
{
    startMarker markerData;
    strncpy( markerData.marker, CONFIG_START_MARKER, sizeof(CONFIG_START_MARKER) );
	markerData = EEPROM.put( 0, markerData );
}


// Erase the content of the flash to 0
void ICACHE_FLASH_ATTR ConfigManager::EraseFlash()
{
    LOG( PSTR("(Config) Erasing flash") );

	for( uint16_t i = 0 ; i < SPI_FLASH_SEC_SIZE ; i++ ) {
		EEPROM.write( i, 0 );
		yield();
	}
}


ConfigManager config;       // Create the global instance
