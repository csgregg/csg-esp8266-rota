/* Info and License

Defines the physical details of the IOT device and the build environment

*/



#ifndef IOT_DEVICE_H

    #define IOT_DEVICE_H


    #define TEXTIFY(...) #__VA_ARGS__
    #define ESCAPEQUOTE(...) TEXTIFY(__VA_ARGS__)
    

    class iot_device {

        public:

            // Get Build Flags

            const String buildTag = ESCAPEQUOTE(BUILD_TAG);
            const String deviceCode = ESCAPEQUOTE(DEVICE_CODE);
            const String deviceName = ESCAPEQUOTE(DEVICE_NAME);
            const String repoName = ESCAPEQUOTE(DEVICE_REPO);
            const String assetService = ESCAPEQUOTE(ASSET_SERVICE);


            const bool logAsSerial = atoi(ESCAPEQUOTE(LOG_AS_SERIAL));
            const bool logAsService = atoi(ESCAPEQUOTE(LOG_AS_SERVICE));
            const uint loggingLevel = atoi(ESCAPEQUOTE(LOG_LEVEL));

            const long monitorBaud = atol(ESCAPEQUOTE(MONITOR_SPEED));

            const String loggingService = ESCAPEQUOTE(LOGGING_SERVICE);
            const String loggingServiceKey = ESCAPEQUOTE(LOGGING_SERVICE_KEY);
            const String loggingGlobalTags = ESCAPEQUOTE(LOGGING_GLOBAL_TAGS);


            const String platform = ESCAPEQUOTE(PLATFORM);
            const String board = ESCAPEQUOTE(BOARD);
            const String framework = ESCAPEQUOTE(FRAMEWORK);

        protected:



    };

    extern iot_device device;

#endif