/* DNS Manager Library

MIT License

Copyright (c) 2020 Chris Gregg

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

Manages DNS Functions

*/


#include "DNSManager.h"
#include "Logger.h"
#include "IOTDevice.h"


void ICACHE_FLASH_ATTR DNSSettings::setDefaults() {
    mode = DNS_DEFAULT_MODE;
    strcpy_P( hostname, flag_DEVICE_CODE );
    mDNS = DNS_DEFAULT_MDNS;
}




void ICACHE_FLASH_ATTR DNSManager::begin(DNSSettings &settings, bool apMode) {

    _settings = &settings;
    _dnsStarted = false;
    _dnsServer.stop();
    MDNS.end();

    if( _settings->mode ) {

        LOG(PSTR("(Network) Starting DNS"));

        if( apMode ) {
            _dnsServer.setTTL(DNS_TTL);
            _dnsStarted = _dnsServer.start( DNS_PORT, PSTR("*"), WiFi.softAPIP() );
        }

        if( _settings->mDNS ) _dnsStarted &= MDNS.begin( _settings->hostname );

        if( _dnsStarted ) LOG_HIGH(PSTR("(Network) DNS Started"));
        else LOG_HIGH(PSTR("(Network) DNS Error"));

	}

};



void DNSManager::handle() {

    if( _dnsStarted ) {
        MDNS.update();
        _dnsServer.processNextRequest();
    }
    
};
