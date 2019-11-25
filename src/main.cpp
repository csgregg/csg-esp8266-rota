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



void setup() {

    logger.begin( http, client );    
    logger.setMode( device.logAsSerial, false, t_logging_level(device.loggingLevel) );

    LOG("Starting Setup()...");

    LOG("Build tag: " + device.buildTag);
    LOG("Device name: " + device.deviceName);
    LOG("Device code: " + device.deviceCode);
    LOG("GitHub Repo: " + device.repoName);
    LOG("Asset service: " + device.assetService);

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

    updater.setup("http://" + device.assetService + "?repo=" + device.repoName, device.deviceCode, device.buildTag, device.updateInterval, device.skipUpdate );
    updater.begin( http, client );
    
}



void loop() {

  delay(5000);

  DEBUG("Looping every 5 seconds");

  updater.handle();

}