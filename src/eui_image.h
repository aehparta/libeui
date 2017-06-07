 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_IMAGE_H
#define EUI_IMAGE_H

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
typedef struct {
    EUI_OBJECT_HEADER
    const eui_image_data_t *data;
} eui_image_t;


/******************************************************************************/
void eui_image_init(eui_image_t *image, const eui_image_data_t *data);
void eui_image_draw(eui_view_t *view, eui_image_t *image);



#endif /* EUI_IMAGE_H - END OF HEADER FILE */
