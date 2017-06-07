 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_VIEW_H
#define EUI_VIEW_H

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
void eui_view_init(eui_context_t *context, eui_view_t *view);
void eui_view_draw(eui_view_t *view);
void eui_view_update_object(eui_view_t *view, eui_object_t *object);
void eui_view_draw_object(eui_view_t *view, eui_object_t *object, int full_redraw);



#endif /* EUI_VIEW_H - END OF HEADER FILE */
