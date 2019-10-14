#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>
#include <FS.h> 

#define CHECK_INTERVAL 120

#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)

// Get Build Flags
const String buildTag = ESCAPEQUOTE(BUILD_TAG);
const String deviceCode = ESCAPEQUOTE(DEVICE_CODE);
const String deviceName = ESCAPEQUOTE(DEVICE_NAME);
const String repoName = ESCAPEQUOTE(DEVICE_REPO);
const String assetService = ESCAPEQUOTE(DEVICE_ASSET_SERVICE);

const char* progSuffix = "-Pv";
const char* FSSuffix = "-Fv";


ESP8266WiFiMulti WiFiMulti;
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



String get_http(String url){
  http.begin(client, url);
  int httpCode = http.GET();
  if((httpCode > 0) && (httpCode == HTTP_CODE_OK)){
    String payload = http.getString();
    http.end();
    return(payload);
  } else {
    http.end();
    return("server error: "+String(httpCode));
  }
} // end http_get

void setup() {

    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Now starting...");
    Serial.println("Build tag: "+ buildTag);
    Serial.println("Device name: " + deviceName);
    Serial.println("Device code: " + deviceCode);
    Serial.println("GitHub Repo: " + repoName);
    Serial.println("Asset service: " + assetService);

    
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

    server.handleClient();
    
    // press nodemcu's flash button
    bool doUpdates = true;
    if ( doUpdates && doUpdateCheck) {
        Serial.println("Update firmware...");
        if((WiFiMulti.run() == WL_CONNECTED)) {

          String assetRequestURL = "http://" + assetService + "?repo=" + repoName;
          String latestTag = get_http(assetRequestURL);

          Serial.println("Lastest version: " + latestTag);
          Serial.println("Current version: " + buildTag);
          
          // Check for update
          if( latestTag == buildTag ){
            Serial.println("HTTP_UPDATE_NO_UPDATES");              
          }
          else {

            // Update SPIFFS file system
            String imageFileRequest = assetRequestURL + "&asset=" + deviceCode + progSuffix + "&tag=" + latestTag;
            Serial.println("FS file request: " + spiffsFileRequest);

            t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(client, spiffsFileRequest);

            switch(ret) {
                case HTTP_UPDATE_FAILED:
                    Serial.printf("File system update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                    Serial.println();
                    break;

                case HTTP_UPDATE_OK:
                    Serial.println("File system updated successfully");
                    break;
            }

            // Update image
            String spiffsFileRequest = assetRequestURL + "&asset=" + deviceCode + FSSuffix + "&tag=" + latestTag;
            Serial.println("Image file request: " + imageFileRequest);

            ret = ESPhttpUpdate.update(client, imageFileRequest);

            switch(ret) {
                case HTTP_UPDATE_FAILED:
                    Serial.printf("Image update failed - Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                    Serial.println();
                    break;

                case HTTP_UPDATE_OK:
                    Serial.println("Image updated cuccessfully");
                    break;
            }
          }
        }
        doUpdateCheck = false;
    }


}