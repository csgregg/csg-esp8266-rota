#include "WebManager.h"


#include "pages/NetworkSettingsPage.h"
#include "pages/AboutPage.h"
#include "pages/IndexPage.h"
#include "pages/SystemPage.h"

#include "WebFiles.h"       // Use flash instead of LittleFS for web files



// Page handlers
PageHandler webpages[] = {
    networksettingspage.handler,
    aboutpage.handler,
    indexpage.handler,
    systempage.handler
};
