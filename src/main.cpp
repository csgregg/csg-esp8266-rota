#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>
#include <FS.h> 
#include <ArduinoJson.h>
#include "credentials.h"

#define CHECK_INTERVAL 60

#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)

#define SKIPUPDATE false

// Get Build Flags
const String buildTag = ESCAPEQUOTE(BUILD_TAG);
const String deviceCode = ESCAPEQUOTE(DEVICE_CODE);
const String deviceName = ESCAPEQUOTE(DEVICE_NAME);
const String repoName = ESCAPEQUOTE(DEVICE_REPO);
const String assetService = ESCAPEQUOTE(ASSET_SERVICE);
const String loggingService = ESCAPEQUOTE(LOGGING_SERVICE);
const String loggingServiceKey = ESCAPEQUOTE(LOGGING_SERVICE_KEY);
const String loggingGlobalTags = ESCAPEQUOTE(LOGGING_GLOBAL_TAGS);


const char* progSuffix = "-Pv";
const char* FSSuffix = "-Fv";

bool LED = true;


ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
ESP8266WebServer server(80);
Ticker updateCheck;
boolean doUpdateCheck = true;

HTTPClient http;   

/* JSON Loggining Format

{
  "localtime": 1234567890,
  "message": "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",
  "Device": {
    "Hardware": {
      "platform": "12345678901234567890",
      "board": "12345678901234567890",
      "framework": "12345678901234567890",
      "MAC": "mm:mm:mm:ss:ss:ss"
    },
    "Env": {
      "Name": "123456789012345678901234567890",
      "Code": "12345678901234567890",
      "Build": "xx.xx.xx",
      "Heap": "1234567890"
     }
  }
}

  Use https://arduinojson.org/v6/assistant/

*/


void LogToCloud(String customTags, String strMessage){
  Serial.println("Logging to Cloud"); 

  if( customTags != "" ) customTags = "," + customTags;

  String loggingServiceRequestURL = "http://" + loggingService + "/" + loggingServiceKey + "/tag/" + loggingGlobalTags + customTags  + "/";

  Serial.print("Connecting to: ");
  Serial.println(loggingServiceRequestURL);
  
  http.begin(client, loggingServiceRequestURL);
  http.addHeader("Content-Type", "content-type:text/plain");
  
  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 382;
  DynamicJsonDocument jsonLog(capacity);
  String jsonMessage;

  jsonLog["localtime"] = millis();

  if(strMessage.length() > 140 ) strMessage = strMessage.substring(0, 140);

  jsonLog["message"] = strMessage;

  JsonObject Device = jsonLog.createNestedObject("Device");

    JsonObject Device_Hardware = Device.createNestedObject("Hardware");
      Device_Hardware["platform"] = "espressif8266";
      Device_Hardware["board"] = "d1_mini";
      Device_Hardware["framework"] = "arduino";

      String tempMAC = WiFi.macAddress();
      Device_Hardware["MAC"] = tempMAC.c_str();

    JsonObject Device_Env = Device.createNestedObject("Env");
      Device_Env["Name"] = deviceName.c_str();
      Device_Env["Code"] = deviceCode.c_str();
      Device_Env["Build"] = buildTag.c_str();

      uint32_t free = system_get_free_heap_size(); // get free ram 
      Device_Env["Heap"] = free;

  serializeJson(jsonLog, jsonMessage);

  Serial.println(jsonMessage);

  int httpCode = http.POST(jsonMessage);

  if( httpCode == HTTP_CODE_OK ) Serial.println("Log to Cloud successful");
  else Serial.println("Error posting log - Error: " + String(httpCode));
 
  http.end();
}



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

  String assetRequestURL = "http://" + assetService + "?repo=" + repoName;
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
    Serial.println("Current version: " + buildTag);
    
    // Check for update
    if( latestTag == buildTag ){
      Serial.println("No new update");              
    }
    else {
      // Update SPIFFS file system
      String spiffsFileRequest = assetRequestURL + "&asset=" + deviceCode + FSSuffix + "&tag=" + latestTag;
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
      String imageFileRequest = assetRequestURL + "&asset=" + deviceCode + progSuffix + "&tag=" + latestTag;
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

    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Now starting...");
    Serial.println("Build tag: "+ buildTag);
    Serial.println("Device name: " + deviceName);
    Serial.println("Device code: " + deviceCode);
    Serial.println("GitHub Repo: " + repoName);
    Serial.println("Asset service: " + assetService);

    pinMode(LED_BUILTIN, OUTPUT);
    
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();

    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();
    Serial.println("Started..");


    Serial.println(WiFi.localIP());

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

  delay(1000);
  // Serial.print(".");
  uint32_t free = system_get_free_heap_size(); // get free ram   
  Serial.println(free); // output ram to serial 

  int tmp = millis();
  LogToCloud("UpDateService", "Debug...");
  Serial.println("Log to Cloud: " + String(millis()-tmp) + "ms");
  Serial.println("");

  digitalWrite(LED_BUILTIN, LED);
  LED = !LED;


  tmp = millis();
  server.handleClient();
    if( (millis()-tmp) >2 ){
    Serial.println("Handle Client: " + String(millis()-tmp) + "ms");
    Serial.println("");
  }

  // press nodemcu's flash button
  bool doUpdates = true;

  if( WiFiMulti.run() == WL_CONNECTED && doUpdates && doUpdateCheck ) {

    Serial.println("");
    tmp = millis();
    UpdateFirmware();
    Serial.println("Update Firmware: " + String(millis()-tmp) + "ms");
    Serial.println("");

    doUpdateCheck = false;
  }

}