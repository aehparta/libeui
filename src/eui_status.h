 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_STATUS_H
#define EUI_STATUS_H

/******************************************************************************/
#include <string.h>
#include "libeui.h"


/******************************************************************************/
void eui_status_init();
void eui_status_set(const char *status);
void eui_status_on();
void eui_status_off();
void eui_status_rtc_to_str(char *str);
void eui_status_draw();
void eui_status_set_bat_level(int level);


#endif /* EUI_STATUS_H - END OF HEADER FILE */
