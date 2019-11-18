#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>
#include <FS.h> 
#include <ArduinoJson.h>
#include "debuglogging.h"
#include "iot_device.h"

#define CHECK_INTERVAL 60



#define SKIPUPDATE true



const char* progSuffix = "-Pv";
const char* FSSuffix = "-Fv";

bool LED = true;


WiFiClient client;
ESP8266WebServer server(80);
Ticker updateCheck;
boolean doUpdateCheck = true;

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




void enableUpdateCheck() {
  doUpdateCheck = true;
}



void UpdateFirmware(){

  Serial.println("Update firmware...");

  String assetRequestURL = "http://" + device.assetService + "?repo=" + device.repoName;
  String latestTag;

  http.begin(client, assetRequestURL);
  int httpCode = http.GET();

  if( httpCode != HTTP_CODE_OK ) {
    http.end();
    Serial.println("Error getting latest release - Error: " + String(httpCode));
  }
  else {
    latestTag = http.getString();
    http.end();
            
    Serial.println("Lastest version: " + latestTag);
    Serial.println("Current version: " + device.buildTag);
    
    // Check for update
    if( latestTag == device.buildTag ){
      Serial.println("No new update");              
    }
    else {
      // Update SPIFFS file system
      String spiffsFileRequest = assetRequestURL + "&asset=" + device.deviceCode + FSSuffix + "&tag=" + latestTag;
      Serial.println("FS file request: " + spiffsFileRequest);

      t_httpUpdate_return ret;

       if( SKIPUPDATE ){
        Serial.println("Skipping update");
        ret = HTTP_UPDATE_NO_UPDATES;
      }
      else ret = ESPhttpUpdate.updateSpiffs(client, spiffsFileRequest);

      switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("File system update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            Serial.println();
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("No new file system update");
            break;
            
        case HTTP_UPDATE_OK:
            Serial.println("File system updated successfully");
            break;
      }

      // Update image
      String imageFileRequest = assetRequestURL + "&asset=" + device.deviceCode + progSuffix + "&tag=" + latestTag;
      Serial.println("Image file request: " + imageFileRequest);

      ESPhttpUpdate.rebootOnUpdate(false);

      if( SKIPUPDATE ){
        Serial.println("Skipping update");
        ret = HTTP_UPDATE_NO_UPDATES;
      }
      else ret = ESPhttpUpdate.update(client, imageFileRequest);

      switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("Image update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            Serial.println();
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("No new image update");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("Image updated cuccessfully");

            Serial.println("Rebooting in 5 sec");
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

 
    logger.println(LOG_INFO, TAG_STATUS, "Starting Setup()...");

    logger.println(LOG_INFO, TAG_STATUS, "Build tag: " + device.buildTag);
    logger.println(LOG_INFO, TAG_STATUS, "Device name: " + device.deviceName);
    logger.println(LOG_INFO, TAG_STATUS, "Device code: " + device.deviceCode);
    logger.println(LOG_INFO, TAG_STATUS, "GitHub Repo: " + device.repoName);
    logger.println(LOG_INFO, TAG_STATUS, "Asset service: " + device.assetService);

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

    logger.println(LOG_INFO, TAG_STATUS, "WiFI Started: " + WiFi.localIP().toString());

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

    //NEW
    SPIFFS.begin(); 

    // don't wanna miss a thing... Check every 120 seconds
    updateCheck.attach(CHECK_INTERVAL, enableUpdateCheck);
}

void loop() {

  delay(10000);

  logger.println(LOG_INFO, TAG_STATUS, "Looping every second");

  // digitalWrite(LED_BUILTIN, LED);
  // LED = !LED;

  if(WiFi.status() == WL_CONNECTED && doUpdateCheck ) {

    logger.println(LOG_INFO, TAG_STATUS, "Updating Firmware...");
    UpdateFirmware();

    doUpdateCheck = false;
  }

}