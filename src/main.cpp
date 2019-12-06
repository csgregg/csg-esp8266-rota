#include <ESP8266WebServer.h>
#include <WiFiManager.h>


#include <FS.h> 

#include "Logger.h"
#include "IOTDevice.h"
#include "ESPRemoteUpdater.h"


WiFiClient client;
HTTPClient http;
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

    LOG(device.getPlatform(true));
    LOG(device.getBoard(true));
    LOG(device.getFramework(true));

    LOG(device.getChipId(true));
    LOG(device.getName(true));
    LOG(device.getCode(true));
    LOG(device.getBuild(true));
    LOG(device.getEnvironment(true));

    LOG(device.getUpdaterRepo(true));
    LOG(device.getUpdaterUser(true));
    LOG(device.getUpdaterService(true));
    LOG(device.getUpdaterToken(true));
    LOG(device.getUpdaterInterval(true));
    LOG(device.getUpdaterSkip(true));

    LOG(device.getLoggerAsSerial(true));
    LOG(device.getLoggerAsService(true));
    LOG(device.getLoggerLogLevel(true));
    LOG(device.getLoggerGlobalTags(true));
    LOG(device.getLoggerService(true));
    LOG(device.getLoggerServiceKey(true));
    LOG(device.getLoggerBaud(true));
    
}


void setup() {

    logger.begin( http, client, device.getLoggerBaud(), device.getLoggerService(), device.getLoggerServiceKey(), device.getLoggerGlobalTags() );    
    logger.setMode( device.getLoggerAsSerial(), false, t_logging_level(device.getLoggerLogLevel()) );
    elaborateBuildFlags();
    delay(1000);

    WiFi.persistent(false);
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");

    delay(1000);

    logger.setMode( device.getLoggerAsSerial(), device.getLoggerAsService(), t_logging_level(device.getLoggerLogLevel()));

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

    PGM_P updaterurl = PSTR("http://" ESCAPEQUOTE(UPDATE_SERVICE) "?repo=" ESCAPEQUOTE(UPDATE_REPO) "&user=" ESCAPEQUOTE(UPDATE_USER) "&token=" ESCAPEQUOTE(UPDATE_TOKEN));
    updater.setup(updaterurl, device.getCode() , device.getBuild(), device.getUpdaterInterval(), device.getUpdaterSkip() );
    updater.begin( http, client );

    LOG(F("Starting loop()"));
    
}



void loop() {

  updater.handle();
  server.handleClient();

}