 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_TOUCH_H
#define EUI_TOUCH_H

/******************************************************************************/
#include "libeui.h"
#include <stdlib.h>


/******************************************************************************/
void eui_touch_init(eui_context_t *context);
void eui_touch_update(eui_context_t *context, eui_coord_t x, eui_coord_t y, int pressed);
void eui_touch_read(eui_context_t *context);



#endif /* EUI_TOUCH_H - END OF HEADER FILE */
