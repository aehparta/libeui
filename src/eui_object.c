/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "libeui.h"
#include "eui_object.h"


/******************************************************************************/
void eui_object_init(eui_object_t *object)
{
    memset(object, 0, sizeof(*object));
    object->fg = -1;
    object->bg = -1;
    EUI_FLAG_ON(object, EUI_FLAG_OPAQUE);
    EUI_FLAG_ON(object, EUI_FLAG_REDRAW);
}


/******************************************************************************/
void eui_object_update(eui_view_t *view, eui_object_t *object)
{
    /* don't go any further, this object has no properties that require
     * anything that this code does later
     */
    if (object->type != EUI_OBJECT_BUTTON && object->type != EUI_OBJECT_IMAGE) {
        return;
    }
    
    /* if object is out of view */
    if (!EUI_RECTS_INTERSECT(&object->rc, &view->context->clip)) {
        return;
    }
    
    if (view->context->touch.state == EUI_FLAG_TOUCH_PRESSED) {
        /* screen is pressed, check if it is this object that is under the pressure */
        if (EUI_RECT_HAS_COORD(&object->rc, view->context->touch.x, view->context->touch.y) &&
            EUI_RECT_HAS_COORD(&view->context->clip, view->context->touch.x, view->context->touch.y)) {
            if (!EUI_FLAG_TOUCH_IS(object, EUI_FLAG_TOUCH_PRESSED)) {
                /* this object is pressed and was not before */
                EUI_FLAG_TOUCH_SET(object, EUI_FLAG_TOUCH_PRESSED);
                EUI_FLAG_ON(object, EUI_FLAG_REDRAW);
                EUI_FLAG_ON(object, EUI_FLAG_INVERSE);
            }
        } else if (EUI_FLAG_TOUCH_IS(object, EUI_FLAG_TOUCH_PRESSED)) {
            /* object lost press focus */
            EUI_FLAG_TOUCH_SET(object, EUI_FLAG_TOUCH_NONE);
            EUI_FLAG_ON(object, EUI_FLAG_REDRAW);
            EUI_FLAG_OFF(object, EUI_FLAG_INVERSE);
        }
    } else if (EUI_FLAG_TOUCH_IS(object, EUI_FLAG_TOUCH_PRESSED)) {
        /* user selected this object by releasing press when on it */
        EUI_FLAG_TOUCH_SET(object, EUI_FLAG_TOUCH_NONE);
        EUI_FLAG_ON(object, EUI_FLAG_REDRAW);
        EUI_FLAG_OFF(object, EUI_FLAG_INVERSE);
        if (view->context->touch.state == EUI_FLAG_TOUCH_RELEASED) {
            if (object->type == EUI_OBJECT_BUTTON) {
                eui_button_t *button = (eui_button_t *)object;
                if (button->callback) {
                    button->callback(button);
                }
            }
        }
    }
}


/******************************************************************************/
eui_coord_t eui_object_min_height(eui_view_t *view, eui_object_t *object)
{
    eui_coord_t min_height = (object->border << 1) + (object->pad_outter << 1);
    
    if (object->type == EUI_OBJECT_BUTTON || object->type == EUI_OBJECT_LABEL) {
        eui_label_t *label = (eui_label_t *)object;
        const eui_font_t *font = view->font;
        
        if (label->font) {
            font = label->font;
        }
        min_height += eui_str_char_max_height(font) + 2;
    }
    
    return min_height;
}


