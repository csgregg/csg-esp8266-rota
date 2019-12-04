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

    static const char platform[] PROGMEM = "Platform: " ESCAPEQUOTE(PLATFORM);
    static const char board[] PROGMEM = "Board: " ESCAPEQUOTE(BOARD);
    static const char framework[] PROGMEM = "Framework: " ESCAPEQUOTE(FRAMEWORK);

    static const char devicename[] PROGMEM = "Device name: " ESCAPEQUOTE(DEVICE_NAME);
    static const char devicecode[] PROGMEM = "Device code: " ESCAPEQUOTE(DEVICE_CODE);

    static const char buildtag[] PROGMEM = "Build tag: " ESCAPEQUOTE(BUILD_TAG);
    static const char buildenv[] PROGMEM = "Build environment: " ESCAPEQUOTE(BUILD_ENV);

    static const char updaterepo[] PROGMEM = "GitHub Repo: " ESCAPEQUOTE(UPDATE_REPO);
    static const char updateuser[] PROGMEM = "GitHub User: " ESCAPEQUOTE(UPDATE_USER);
    static const char updatetoken[] PROGMEM = "GitHub Token: " ESCAPEQUOTE(UPDATE_TOKEN);
    static const char updateservice[] PROGMEM = "Asset service: " ESCAPEQUOTE(UPDATE_SERVICE);
    static const char updateinterval[] PROGMEM = "Update interval: " ESCAPEQUOTE(UPDATE_INTERVAL);
    static const char updateskip[] PROGMEM = "Skipping update: " ESCAPEQUOTE(UPDATE_INTERVAL);

    static const char logasserial[] PROGMEM = "Log as serial: " ESCAPEQUOTE(LOG_AS_SERIAL);
    static const char logasservice[] PROGMEM = "Log as service: " ESCAPEQUOTE(LOG_AS_SERVICE);
    static const char loglevel[] PROGMEM = "Logging level: " ESCAPEQUOTE(LOG_LEVEL);

    static const char loggingservice[] PROGMEM = "Logging service: " ESCAPEQUOTE(LOGGING_SERVICE);
    static const char loggingservicekey[] PROGMEM = "Logging service key: " ESCAPEQUOTE(LOGGING_SERVICE_KEY);
    static const char loggingservicetags[] PROGMEM = "Logging global tags: " ESCAPEQUOTE(LOGGING_GLOBAL_TAGS);

    static const char monitorbaud[] PROGMEM = "Monitor baud: " ESCAPEQUOTE(MONITOR_SPEED);

    LOG( platform );
    LOG( board );
    LOG( framework );

    LOG( devicename );
    LOG( devicecode );

    LOG( buildtag );
    LOG( buildenv );

    LOG( updaterepo );
    LOG( updateuser );
    LOG( updatetoken );
    LOG( updateservice );
    LOG( updateinterval );
    LOG( updateskip );

    LOG( logasserial );
    LOG( logasservice );
    LOG( loglevel );

    LOG( loggingservice );
    LOG( loggingservicekey );
    LOG( loggingservicetags );

    LOG( monitorbaud );
    
}



void setup() {

    logger.begin( http, client );    
    logger.setMode( B_LOG_AS_SERIAL, false, t_logging_level(U_LOGGING_LEVEL) );

    delay(1000);

    WiFi.persistent(false);
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");

    delay(1000);

    logger.setMode( B_LOG_AS_SERIAL, B_LOG_AS_SERVICE, t_logging_level(U_LOGGING_LEVEL) );

    LOG("WiFI Started: " + WiFi.localIP().toString());

    elaborateBuildFlags();

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

    static const char updaterurl[] PROGMEM = "http://" ESCAPEQUOTE(UPDATE_SERVICE) "?repo=" ESCAPEQUOTE(UPDATE_REPO) "&user=" ESCAPEQUOTE(UPDATE_USER) "&token=" ESCAPEQUOTE(UPDATE_TOKEN);
    updater.setup(updaterurl, FPSTR(STR_DEVICE_CODE_P) , FPSTR(STR_BUILD_TAG_P), L_UPDATEINTERVAL, B_SKIPUPDATES );
    updater.begin( http, client );

    LOG(F("Starting loop()"));
    
}



void loop() {

  updater.handle();
  server.handleClient();

}