 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_OBJECT_H
#define EUI_OBJECT_H

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
void eui_object_init(eui_object_t *object);
void eui_object_update(eui_view_t *view, eui_object_t *object);
eui_coord_t eui_object_min_height(eui_view_t *view, eui_object_t *object);


#endif /* EUI_OBJECT_H - END OF HEADER FILE */
