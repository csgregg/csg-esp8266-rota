#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>
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

String get_env_var( String const & key ) {                                 
    char * val;                                                                        
    val = getenv( key.c_str() );                                                       
    String retval = "";                                                           
    if (val != NULL) {                                                                 
        retval = val;                                                                    
    }                                                                                  
    return retval;                                                                        
}     


void elaborateBuildFlags() {

    LOG(get_env_var("PLATFORMIO_CORE_DIR"));

    LOG("Platform: " + device.platform);
    LOG("Board: " + device.board);
    LOG("Framework: " + device.framework);

    LOG("Device name: " + device.deviceName);
    LOG("Device code: " + device.deviceCode);

    LOG("Build tag: " + device.buildTag);
    LOG("Build environment: " + device.buildEnv);
    LOG("GitHub Repo: " + device.repoName);
    LOG("Asset service: " + device.assetService);

    logger.setTypeTag(LOG_NORMAL, TAG_STATUS);
    logger.printf("Update interval: %f", device.updateInterval );
    logger.printf("Skip update: %i", device.skipUpdates );
    
    logger.printf("Log as serial: %i", device.logAsSerial);
    logger.printf("Log as service: %i", device.logAsService);
    logger.printf("Logging level: %i", device.loggingLevel);

    LOG("Logging service: " + device.loggingService);
    LOG("Logging serice key: " + device.loggingServiceKey);
    LOG("Logging global tags: " + device.loggingGlobalTags);

    logger.printf("Monitor baud: %i", device.monitorBaud );
    
}



void setup() {

    logger.begin( http, client );    
    logger.setMode( device.logAsSerial, false, t_logging_level(device.loggingLevel) );

    LOG("Starting Setup()...");

    elaborateBuildFlags();

    delay(1000);
    // pinMode(LED_BUILTIN, OUTPUT);

    WiFi.persistent(false);
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");

    delay(1000);

    logger.setMode( device.logAsSerial, device.logAsService, t_logging_level(device.loggingLevel) );

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

    updater.setup("http://" + device.assetService + "?repo=" + device.repoName, device.deviceCode, device.buildTag, device.updateInterval, device.skipUpdates );
    updater.begin( http, client );
    
}



void loop() {

  delay(5000);

  DEBUG("Looping every 5 seconds");

  updater.handle();

}