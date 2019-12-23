#include <ESP8266WebServer.h>
#include <WiFiManager.h>


#include <FS.h> 

#include "Logger.h"
#include "IOTDevice.h"
#include "ESPRemoteUpdater.h"
#include "ConfigManager.h"
#include "LiteralManager.h"




WiFiClient client;
ESP8266WebServer server(80);



String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)


String ICACHE_FLASH_ATTR getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".html.gz")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".css.gz")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".js.gz")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".ico.gz")) return "image/x-icon";
  else if (filename.endsWith(".woff")) return "application/font-woff";
  else if (filename.endsWith(".woff.gz")) return "application/font-woff";
  return "text/plain";
}

// send the right file to the client (if it exists)
bool ICACHE_FLASH_ATTR handleFileRead(String shortpath) {

  Serial.println("handleFileRead: " + shortpath);
  if (shortpath.endsWith("/")) shortpath += "index.html";         // If a folder is requested, send the index file

  shortpath.replace("fonts/glyphicons-halflings-regular", "font");

  String path = shortpath + ".gz";

  Serial.println(path);

  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();       
    Serial.println(sent);                                // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}




void ICACHE_FLASH_ATTR elaborateBuildFlags() {

    LOG(device.getChipId(true));
    
    LOG(device_getBuildFlag(flag_PLATFORM, true));
    LOG(device_getBuildFlag(flag_BOARD,true));
    LOG(device_getBuildFlag(flag_FRAMEWORK,true));

    LOG(device_getBuildFlag(flag_DEVICE_NAME,true));
    LOG(device_getBuildFlag(flag_DEVICE_CODE,true));
    LOG(device_getBuildFlag(flag_BUILD_TAG,true));
    LOG(device_getBuildFlag(flag_BUILD_ENV,true));
    LOG(device_getBuildFlag(flag_BUILD_PLATFORM,true));
    LOG(device_getBuildFlag(flag_BUILD_NO,true));
    LOG(device_getBuildFlag(flag_BUILD_TIMESTAMP,true));

    LOG(device_getBuildFlag(flag_UPDATER_REPO,true));
    LOG(device_getBuildFlag(flag_UPDATER_USER,true));
    LOG(device_getBuildFlag(flag_UPDATER_TOKEN,true));
    LOG(device_getBuildFlag(flag_UPDATER_SERVICE,true));
    LOG(device_getBuildFlag(flag_UPDATER_INTERVAL,true));
    LOG(device_getBuildFlag(flag_UPDATER_SKIP,true));

    LOG(device_getBuildFlag(flag_LOGGER_LEVEL,true));
    LOG(device_getBuildFlag(flag_LOGGER_AS_SERIAL,true));
    LOG(device_getBuildFlag(flag_LOGGER_AS_SERVICE,true));
    LOG(device_getBuildFlag(flag_LOGGER_SERVICE,true));
    LOG(device_getBuildFlag(flag_LOGGER_SERVICE_KEY,true));
    LOG(device_getBuildFlag(flag_LOGGER_GLOBAL_TAGS,true));
    LOG(device_getBuildFlag(flag_MONITOR_SPEED,true));
       
}


void ICACHE_FLASH_ATTR setup() {

    logger.begin( client, device_getBuildFlag(flag_MONITOR_SPEED), device_getBuildFlag(flag_LOGGER_SERVICE), device_getBuildFlag(flag_LOGGER_SERVICE_KEY), device_getBuildFlag(flag_LOGGER_GLOBAL_TAGS) );    
    logger.setMode( device_getBuildFlag(flag_LOGGER_AS_SERIAL), false, loggingLevel(device_getBuildFlag(flag_LOGGER_LEVEL)) );

    config.Initialize();
    //config.ResetToDefaults();

        elaborateBuildFlags();

        DEBUG_STOP();

    WiFi.persistent(false);
    WiFiManager wifiManager;
    wifiManager.autoConnect( config.Settings.apSSID, config.Settings.apPwd );

    logger.setMode( device_getBuildFlag(flag_LOGGER_AS_SERIAL), device_getBuildFlag(flag_LOGGER_AS_SERVICE), loggingLevel(device_getBuildFlag(flag_LOGGER_LEVEL)) );





    LOG("WiFI Started: " + WiFi.localIP().toString());

    server.begin(); 

    server.onNotFound([]() {                              // If the client requests any URI
      if (!handleFileRead(server.uri()))                  // send it if it exists
        server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });


    SPIFFS.begin(); 

    updater.setup( device_getBuildFlag(flag_UPDATER_SERVICE), device_getBuildFlag(flag_UPDATER_REPO), device_getBuildFlag(flag_UPDATER_USER), device_getBuildFlag(flag_UPDATER_TOKEN), device_getBuildFlag(flag_DEVICE_CODE), device_getBuildFlag(flag_BUILD_TAG), device_getBuildFlag(flag_UPDATER_INTERVAL), device_getBuildFlag(flag_UPDATER_SKIP) );
    updater.begin( client );
    
    LOG(F("Starting loop()"));
    
}



void loop() {

  updater.handle();
  server.handleClient();

}