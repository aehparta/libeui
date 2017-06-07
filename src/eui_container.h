 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_CONTAINER_H
#define EUI_CONTAINER_H

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
void eui_container_init(eui_container_t *container);

void eui_vbox_init(eui_box_t *container);
void eui_hbox_init(eui_box_t *container);
void eui_area_init(eui_area_t *container);

void eui_container_append(eui_container_t *container, eui_object_t *object);

void eui_container_calculate(eui_view_t *view, eui_container_t *container);


#endif /* EUI_CONTAINER_H - END OF HEADER FILE */
