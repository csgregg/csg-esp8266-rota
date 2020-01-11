#include <Arduino.h>
#include <pgmspace.h>

#include "website.h"


// Public :



// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LITERALS)
    WebsiteManager website;
#endif