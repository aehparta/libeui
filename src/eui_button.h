 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_BUTTON_H
#define EUI_BUTTON_H

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[32];
    void (*callback)(void *);
    void *userdata;
} eui_button_t;


/******************************************************************************/
void eui_button_init(eui_button_t *button, char *text);
void eui_button_set_text(eui_button_t *button, char *text);

void eui_button_draw(eui_view_t *view, eui_button_t *button);



#endif /* EUI_BUTTON_H - END OF HEADER FILE */
