/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_view.h"


/******************************************************************************/
void eui_view_init(eui_context_t *context, eui_view_t *view)
{
    memset(view, 0, sizeof(*view));
    view->context = context;
    view->rc.x1 = 0;
    view->rc.y1 = 0;
    view->rc.x2 = view->rc.x1 + context->w - 1;
    view->rc.y2 = view->rc.y1 + context->h - 1;
    view->fg = EUI_COLOR_WHITE; /* white */
    view->bg = EUI_COLOR_BLACK; /* black */
}


/******************************************************************************/
void eui_view_draw(eui_view_t *view)
{
    eui_set_clip_region(view->context, &view->rc);
    view->object->rc = view->rc;
    
    eui_view_update_object(view, view->object);
    eui_view_draw_object(view, view->object, 0);
}


/******************************************************************************/
void eui_view_update_object(eui_view_t *view, eui_object_t *object)
{
    /* failsafe */
    if (!object) {
        return;
    }
    
    if (object->type < EUI_OBJECT_CONTAINERS) {
        /* check if object is pressed etc */
        eui_object_update(view, object);
    } else {
        /* if type is container, update children */
        eui_container_t *container = (eui_container_t *)object;
        eui_object_t *child = container->object;
        
        /* calculate size of children */
        if (EUI_FLAG_IS(object, EUI_FLAG_REDRAW) ||
            (view->context->touch.state_cache == EUI_FLAG_TOUCH_DRAGGED ||
             view->context->touch.state == EUI_FLAG_TOUCH_DRAG_RELEASED)) {
            switch (object->type) {
            case EUI_OBJECT_VBOX:
            case EUI_OBJECT_HBOX:
            case EUI_OBJECT_AREA:
                eui_container_calculate(view, container);
                break;
                
            default:
                return;
            }
        }
        
        /* update children */
        while (child) {
            eui_set_clip_region(view->context, &object->rc);
            eui_view_update_object(view, child);
            child = child->next;
        }
    }
}


/******************************************************************************/
void eui_view_draw_object(eui_view_t *view, eui_object_t *object, int full_redraw)
{
    eui_rect_t rc;
    int i;
    
    /* failsafe */
    if (!object) {
        return;
    }
    
    /* if object is out of view */
    if (!EUI_RECTS_INTERSECT(&object->rc, &view->context->clip)) {
        return;
    }
    
    /* if object should be (re)drawn */
    if (EUI_FLAG_IS(object, EUI_FLAG_REDRAW) || full_redraw) {
        /* draw background if object is opaque */
        if (EUI_FLAG_IS(object, EUI_FLAG_OPAQUE)) {
            if (object->bg >= 0) {
                if (!EUI_FLAG_IS(object, EUI_FLAG_INVERSE)) {
                    eui_set_fg(view->context, object->bg);
                } else {
                    eui_set_fg(view->context, object->fg);
                }
            } else {
                if (!EUI_FLAG_IS(object, EUI_FLAG_INVERSE)) {
                    eui_set_fg(view->context, view->bg);
                } else {
                    eui_set_fg(view->context, view->fg);
                }
            }
            eui_fill_rect(view->context, &object->rc);
            
            /* disable background fill for containers */
            if (object->type > EUI_OBJECT_CONTAINERS) {
                EUI_FLAG_OFF(object, EUI_FLAG_OPAQUE);
            }
        }
    
        /* draw normal objects */
        if (object->type < EUI_OBJECT_CONTAINERS) {
            /* set background and foreground colors */    
            if (object->fg >= 0) {
                if (!EUI_FLAG_IS(object, EUI_FLAG_INVERSE)) {
                    eui_set_fg(view->context, object->fg);
                } else {
                    eui_set_fg(view->context, object->bg);
                }
            } else {
                if (!EUI_FLAG_IS(object, EUI_FLAG_INVERSE)) {
                    eui_set_fg(view->context, view->fg);
                } else {
                    eui_set_fg(view->context, view->bg);
                }
            }
            if (object->bg >= 0) {
                if (!EUI_FLAG_IS(object, EUI_FLAG_INVERSE)) {
                    eui_set_bg(view->context, object->bg);
                } else {
                    eui_set_bg(view->context, object->fg);
                }
            } else {
                if (!EUI_FLAG_IS(object, EUI_FLAG_INVERSE)) {
                    eui_set_bg(view->context, view->bg);
                } else {
                    eui_set_bg(view->context, view->fg);
                }
            }
        
            if (object->type == EUI_OBJECT_BUTTON ||
                object->type == EUI_OBJECT_LABEL) {
                eui_label_draw(view, (eui_label_t *)object);
            } else if (object->type == EUI_OBJECT_IMAGE) {
                eui_image_draw(view, (eui_image_t *)object);
            }
        }
        
        /* draw border */
        if (object->border > 0) {
            for (i = 0; i < object->border; i++) {
                rc.x1 = object->rc.x1 + object->pad_outter + i;
                rc.y1 = object->rc.y1 + object->pad_outter + i;
                rc.x2 = object->rc.x2 - object->pad_outter - i;
                rc.y2 = object->rc.y2 - object->pad_outter - i;
                eui_draw_rect(view->context, &rc);
            }
        }
    }
    
    /* if type is container, draw children */
    if (object->type > EUI_OBJECT_CONTAINERS) {
        eui_container_t *container = (eui_container_t *)object;
        eui_object_t *child = container->object;
        
        /* draw children */
        while (child) {
            eui_set_clip_region(view->context, &object->rc);
            eui_view_draw_object(view, child, full_redraw);
            child = child->next;
        }
    }
    
    EUI_FLAG_OFF(object, EUI_FLAG_REDRAW);
}

