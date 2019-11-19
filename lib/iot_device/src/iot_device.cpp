/* Library Info and License
*/

#include <Arduino.h>

#include "iot_device.h"
#include "credentials.h"


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_IOT_DEVICE)
    iot_device device;
#endif