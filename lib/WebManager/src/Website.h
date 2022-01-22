#include "WebManager.h"


#include "NetworkSettingsPage.h"
#include "AboutPage.h"
#include "IndexPage.h"
#include "SystemPage.h"

#include "WebFiles.h"       // Use flash instead of LittleFS for web files



// Page handlers
PageHandler webpages[] = {
    networksettingspage.handler,
    aboutpage.handler,
    indexpage.handler,
    systempage.handler
};
