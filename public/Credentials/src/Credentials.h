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
// RUN: travis encrypt LOGGER_SERVICE_KEY=<<Secret>> --add env.global
// PLATFORMIO.INI: -DLOGGER_SERVICE_KEY=${sysenv.LOGGING_SERVICE_KEY}
#ifndef LOGGER_SERVICE_KEY
    #define LOGGER_SERVICE_KEY "KEY"
#endif

// Remote Update
// RUN: travis encrypt UPDATER_TOKEN=<<Secret>> --add env.global
// PLATFORMIO.INI: -DUPDATER_TOKEN=${sysenv.UPDATE_TOKEN}
#ifndef UPDATER_TOKEN
    #define UPDATER_TOKEN "TOKEN"
#endif