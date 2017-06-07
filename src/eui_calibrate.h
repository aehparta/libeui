 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_CALIBRATE_H
#define EUI_CALIBRATE_H

/******************************************************************************/
#include <stdio.h>
#include "libeui.h"


/******************************************************************************/
/** Internal call to initialize this module. */
void eui_calibrate_init();

/**
 * Call this from main loop to run calibration.
 *
 * @return 0 when this should be still called, 1 when finished calibration
 */
int eui_calibrate_do();



#endif /* EUI_CALIBRATE_H - END OF HEADER FILE */
