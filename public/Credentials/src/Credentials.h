// Public copy of secrets - don't store anything here. Use copy in private folder instead


// Local WiFi
#ifndef WIFI_SSID
    #define WIFI_SSID "WIFI_SSID"
#endif
#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "WIFI_PASSWORD"
#endif


// Environment variables encrypted using Travis CLI

// Logging Service
// RUN: travis encrypt LOGGING_SERVICE_KEY=<<Secret>> --add env.global
// PLATFORMIO.INI: -DLOGGING_SERVICE_KEY=${sysenv.LOGGING_SERVICE_KEY}
#ifndef LOGGING_SERVICE_KEY
    #define LOGGING_SERVICE_KEY ""
#endif

