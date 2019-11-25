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

#define CHECK_INTERVAL 60

#define SKIPUPDATE true


const char* progSuffix = "-Pv";
const char* FSSuffix = "-Fv";

bool LED = true;

WiFiClient client;
ESP8266WebServer server(80);
HTTPClient http;




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



void UpdateFirmware(){

  LOG("Update firmware...");

  String assetRequestURL = "http://" + device.assetService + "?repo=" + device.repoName;
  String latestTag;

  http.begin(client, assetRequestURL);
  int httpCode = http.GET();

  if( httpCode != HTTP_CODE_OK ) {
    http.end();
    DEBUG("Error getting latest release - Error: " + String(httpCode));
  }
  else {

    // NEED TO CHANGE 
    // For stuations like a captive portal redirect which returns an alternative page redirect. The page content gets loaded as the latest tag which is bad
    // Suggest - githubfetchasset needs to return JSON which can then be confirmed and interpreted. 
    
    latestTag = http.getString();
    http.end();
            
    LOG("Lastest version: " + latestTag);
    LOG("Current version: " + device.buildTag);
    
    // Check for update
    if( latestTag == device.buildTag ){
      LOG("No new update");              
    }
    else {
      // Update SPIFFS file system
      String spiffsFileRequest = assetRequestURL + "&asset=" + device.deviceCode + FSSuffix + "&tag=" + latestTag;
      LOG("FS file request: " + spiffsFileRequest);

      t_httpUpdate_return ret;

       if( SKIPUPDATE ){
        LOG("Skipping update");
        ret = HTTP_UPDATE_NO_UPDATES;
      }
      else ret = ESPhttpUpdate.updateSpiffs(client, spiffsFileRequest);

      switch(ret) {
        case HTTP_UPDATE_FAILED:
            logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);
            logger.printf("File system update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            LOG("No new file system update");
            break;
            
        case HTTP_UPDATE_OK:
            LOG("File system updated successfully");
            break;
      }

      // Update image
      String imageFileRequest = assetRequestURL + "&asset=" + device.deviceCode + progSuffix + "&tag=" + latestTag;
      LOG("Image file request: " + imageFileRequest);

      ESPhttpUpdate.rebootOnUpdate(false);

      if( SKIPUPDATE ){
        LOG("Skipping update");
        ret = HTTP_UPDATE_NO_UPDATES;
      }
      else ret = ESPhttpUpdate.update(client, imageFileRequest);

      switch(ret) {
        case HTTP_UPDATE_FAILED:
            logger.setTypeTag(LOG_CRITICAL, TAG_STATUS);
            logger.printf("Image update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            LOG("No new image update");
            break;

        case HTTP_UPDATE_OK:
            LOG("Image updated cuccessfully");

            logger.println(LOG_CRITICAL, TAG_STATUS, "Rebooting in 5 sec");
            delay(5000);
            ESP.restart();

            break;
      }
    }
  }
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
    
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();

    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

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

    updater.setup("http://" + device.assetService + "?repo=" + device.repoName, device.deviceCode, device.buildTag, CHECK_INTERVAL );
    updater.begin( http, client );
    
}


void loop() {

  delay(5000);

  DEBUG("Looping every 5 seconds");

  updater.handle();

}