/**
 * @file        DNSManager.cpp
 * @author      Chris Gregg
 * 
 * @brief       Manages DNS Functions
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
#include "DNSManager.h"
#include "Logger.h"
#include "Env.h"


////////////////////////////////////////////
//// DNS Settings Class

// Public:

// Resets DNS settings to default
void ICACHE_FLASH_ATTR DNSSettings::setDefaults() {
    dnsEnabled = DNS_DEFAULT_MODE;
    strcpy_P( hostName, flag_DEVICE_CODE );
    mDnsEnabled = DNS_DEFAULT_MDNS;
}


////////////////////////////////////////////
//// DNS Manager Class

// Public:

// Initalizes the DNS services
void ICACHE_FLASH_ATTR DNSManager::Begin( DNSSettings& settings, bool isInApMode ) {

    _settings = &settings;
    _dnsStarted = false;
    _dnsServer.stop();
    MDNS.end();

    if( _settings->dnsEnabled ) {

        LOG(PSTR("(Network) Starting DNS"));

        if( isInApMode ) {
            _dnsServer.setTTL( DNS_TTL );
            _dnsStarted = _dnsServer.start( DNS_PORT, PSTR("*"), WiFi.softAPIP() );
        }

        if( _settings->mDnsEnabled ) _dnsStarted &= MDNS.begin( _settings->hostName );

        if( _dnsStarted ) LOG_HIGH(PSTR("(Network) DNS Started"));
        else LOG_HIGH(PSTR("(Network) DNS Error"));

	}

};


// Handle repeating DNS tasks
void DNSManager::Handle() {

    if( _dnsStarted ) {
        MDNS.update();                          // Handle mDNS
        _dnsServer.processNextRequest();        // Handle DNS
    }
    
};
