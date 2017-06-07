/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_button.h"


/******************************************************************************/
void eui_button_init(eui_button_t *button, char *text)
{
    memset(button, 0, sizeof(*button));
    eui_object_init((eui_object_t *)button);
    
    button->type = EUI_OBJECT_BUTTON;
    button->border = 1;
    button->pad_outter = 1;
    button->callback = NULL;
    strcpy(button->label, text);
}

