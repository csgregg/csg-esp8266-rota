#include <Arduino.h>
#include <pgmspace.h>

#include "website.h"


// Public :



void update_thispage() {
    thispage.check_d.setValue(thispage.check.isChecked() ? "checked" : "not checked");
}


void update_thatpage() {
    thatpage.check_d.setValue(thatpage.check.isChecked() ? "checked" : "not checked");
}




// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LITERALS)
    this_webpage thispage;
    that_webpage thatpage;
#endif