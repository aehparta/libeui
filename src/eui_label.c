/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_label.h"


/******************************************************************************/
void eui_label_init(eui_label_t *label, const char *text)
{
    memset(label, 0, sizeof(*label));
    eui_object_init((eui_object_t *)label);
    
    label->type = EUI_OBJECT_LABEL;
    strcpy(label->label, text);
}


/******************************************************************************/
void eui_label_set_text(eui_label_t *label, const char *text)
{
    strcpy(label->label, text);
    EUI_FLAG_ON(label, EUI_FLAG_REDRAW);
}


/******************************************************************************/
void eui_label_draw(eui_view_t *view, eui_label_t *label)
{
    eui_coord_t x, y;
    
    /* set font */
    if (label->font) {
        eui_set_font(view->context, label->font);
    } else {
        eui_set_font(view->context, view->font);
    }
    
    x = label->rc.x1 + (EUI_RECT_W(&label->rc) >> 1);
    y = label->rc.y1 + (EUI_RECT_H(&label->rc) >> 1);
    eui_str_draw_centered(view->context, label->label, -1, x, y, 0);
}
