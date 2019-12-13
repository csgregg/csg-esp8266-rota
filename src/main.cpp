#include <ESP8266WebServer.h>
#include <WiFiManager.h>


#include <FS.h> 

#include "Logger.h"
#include "IOTDevice.h"
#include "ESPRemoteUpdater.h"




WiFiClient client;
ESP8266WebServer server(80);


void filefont1()
{
  File file = SPIFFS.open("/font.woff.gz", "r"); 
  size_t sent = server.streamFile(file, "application/font-woff");
  sent = sent;
}


void fileindex()
{
  File file = SPIFFS.open("/index.html.gz", "r"); 
  size_t sent = server.streamFile(file, "text/html");
  sent = sent;
}


void bootstrap()
{
  File file = SPIFFS.open("/bootstrap.min.css.gz", "r"); 
  size_t sent = server.streamFile(file, "text/css");
  sent = sent;
}


void bootstrapmin()
{
  File file = SPIFFS.open("/bootstrap.min.js.gz", "r"); 
  size_t sent = server.streamFile(file, "application/javascript");
  sent = sent;
}


void jquerymin()
{
  File file = SPIFFS.open("/jquery-3.4.1.min.js.gz", "r"); 
  size_t sent = server.streamFile(file, "application/javascript");
  sent = sent;
}





void elaborateBuildFlags() {

    LOG(device.getChipId(true));
    
    LOG(device_getBuildFlag(flag_PLATFORM, true));
    LOG(device_getBuildFlag(flag_BOARD,true));
    LOG(device_getBuildFlag(flag_FRAMEWORK,true));

    LOG(device_getBuildFlag(flag_DEVICE_NAME,true));
    LOG(device_getBuildFlag(flag_DEVICE_CODE,true));
    LOG(device_getBuildFlag(flag_BUILD_TAG,true));
    LOG(device_getBuildFlag(flag_BUILD_ENV,true));

    LOG(device_getBuildFlag(flag_UPDATER_REPO,true));
    LOG(device_getBuildFlag(flag_UPDATER_USER,true));
    LOG(device_getBuildFlag(flag_UPDATER_TOKEN,true));
    LOG(device_getBuildFlag(flag_UPDATER_SERVICE,true));
    LOG(device_getBuildFlag(flag_UPDATER_INTERVAL,true));
    LOG(device_getBuildFlag(flag_UPDATER_SKIP,true));

    LOG(device_getBuildFlag(flag_LOGGER_LEVEL,true));
    LOG(device_getBuildFlag(flag_LOGGVER_AS_SERIAL,true));
    LOG(device_getBuildFlag(flag_LOGGER_AS_SERVICE,true));
    LOG(device_getBuildFlag(flag_LOGGER_SERVICE,true));
    LOG(device_getBuildFlag(flag_LOGGER_SERVICE_KEY,true));
    LOG(device_getBuildFlag(flag_LOGGER_GLOBAL_TAGS,true));
    LOG(device_getBuildFlag(flag_MONITOR_SPEED,true));
       
}


void setup() {

    logger.begin( client, device_getBuildFlag(flag_MONITOR_SPEED), device_getBuildFlag(flag_LOGGER_SERVICE), device_getBuildFlag(flag_LOGGER_SERVICE_KEY), device_getBuildFlag(flag_LOGGER_GLOBAL_TAGS) );    
    logger.setMode( device_getBuildFlag(flag_LOGGVER_AS_SERIAL), false, t_logging_level(device_getBuildFlag(flag_LOGGER_LEVEL)) );

    delay(1000);

    WiFi.persistent(false);
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");

    delay(1000);

    logger.setMode( device_getBuildFlag(flag_LOGGVER_AS_SERIAL), device_getBuildFlag(flag_LOGGER_AS_SERVICE), t_logging_level(device_getBuildFlag(flag_LOGGER_LEVEL)) );

    elaborateBuildFlags();

    LOG("WiFI Started: " + WiFi.localIP().toString());

    server.begin(); 
    server.on("/", fileindex);
    server.on("/index.html", fileindex);
    server.on("/bootstrap.min.css", bootstrap);
    server.on("bootstrap.min.css", bootstrap);
    server.on("/bootstrap.min.js", bootstrapmin);
    server.on("bootstrap.min.js", bootstrapmin);
    server.on("/jquery-3.4.1.min.js", jquerymin);
    server.on("jquery-3.4.1.min.js", jquerymin);
    server.on("/fonts/glyphicons-halflings-regular.woff", filefont1);
    server.on("fonts/glyphicons-halflings-regular.woff", filefont1);

    SPIFFS.begin(); 



    updater.setup( device_getBuildFlag(flag_UPDATER_SERVICE), device_getBuildFlag(flag_UPDATER_REPO), device_getBuildFlag(flag_UPDATER_USER), device_getBuildFlag(flag_UPDATER_TOKEN), device_getBuildFlag(flag_DEVICE_CODE), device_getBuildFlag(flag_BUILD_TAG), device_getBuildFlag(flag_UPDATER_INTERVAL), device_getBuildFlag(flag_UPDATER_SKIP) );
    updater.begin( client );
    
    LOG(F("Starting loop()"));
    
}



void loop() {

  updater.handle();
  server.handleClient();

}