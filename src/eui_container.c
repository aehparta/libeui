/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_container.h"


/******************************************************************************/
void eui_container_init(eui_container_t *container)
{
    memset(container, 0, sizeof(*container));
    container->fg = -1;
    container->bg = -1;
    container->child_w = 0;
    container->child_h = 0;
    container->offset_x = 0;
    container->offset_x_temp = 0;
    container->offset_y = 0;
    container->offset_y_temp = 0;
    EUI_FLAG_ON(container, EUI_FLAG_OPAQUE);
    EUI_FLAG_ON(container, EUI_FLAG_REDRAW);
}


/******************************************************************************/
int eui_container_count_children(eui_container_t *container)
{
    int i;
    eui_object_t *child = container->object;
    
    if (!child) {
        return 0;
    }
    
    for (i = 1; child->next; i++) {
        child = child->next;
    }
    
    return i;
}


/******************************************************************************/
void eui_vbox_init(eui_box_t *container)
{
    eui_container_init((eui_container_t *)container);
    container->type = EUI_OBJECT_VBOX;
}


/******************************************************************************/
void eui_hbox_init(eui_box_t *container)
{
    eui_container_init((eui_container_t *)container);
    container->type = EUI_OBJECT_HBOX;
}


/******************************************************************************/
void eui_area_init(eui_area_t *container)
{
    eui_container_init((eui_container_t *)container);
    container->type = EUI_OBJECT_AREA;
}


/******************************************************************************/
void eui_container_append(eui_container_t *container, eui_object_t *object)
{
    eui_object_t *child = container->object;
    
    if (!child) {
        container->object = object;
    } else {
        while (child->next) {
            child = child->next;
        }
        child->next = object;
    }
}


/******************************************************************************/
static void eui_base_calculate(eui_view_t *view, eui_container_t *container,
                               eui_coord_t x_fixed, eui_coord_t y_fixed,
                               eui_coord_t w_fixed, eui_coord_t h_fixed,
                               eui_coord_t *w_total, eui_coord_t *h_total,
                               int this_dragged)
{
    eui_object_t *child;
    eui_coord_t x = container->rc.x1;
    eui_coord_t y = container->rc.y1;
    
    /* set size of child objects */
    for (child = container->object; child; child = child->next) {
        /* horizontal setup */
        if (x_fixed != EUI_COORD_NONE) {
            child->rc.x1 = x_fixed;
        } else {
            child->rc.x1 = x;
        }
        if (child->w_fixed > 0) {
            child->rc.x2 = child->rc.x1 + child->w_fixed - 1;
            x += child->w_fixed;
            *w_total += child->w_fixed;
        } else {
            child->rc.x2 = child->rc.x1 + w_fixed - 1;
            x += w_fixed;
            *w_total += w_fixed;
        }
        
        /* vertical setup */
        if (y_fixed != EUI_COORD_NONE) {
            child->rc.y1 = y_fixed;
        } else {
            child->rc.y1 = y;
        }
        if (child->h_fixed > 0) {
            child->rc.y2 = child->rc.y1 + child->h_fixed - 1;
            y += child->h_fixed;
            *h_total += child->h_fixed;
        } else {
            eui_coord_t min = eui_object_min_height(view, child);
            if (min < h_fixed) {
                min = h_fixed;
            }
            child->rc.y2 = child->rc.y1 + min - 1;
            y += min;
            *h_total += min;
        }
    }
}


/******************************************************************************/
void eui_container_calculate(eui_view_t *view, eui_container_t *container)
{
    eui_coord_t x;
    eui_coord_t y;
    eui_object_t *child;
    eui_coord_t total_w = 0;
    eui_coord_t total_h = 0;
    eui_coord_t child_w = 0;
    eui_coord_t child_h = 0;
    eui_coord_t child_x = 0;
    eui_coord_t child_y = 0;
    int n_children_fixed_x = 0;
    int n_children_fixed_y = 0;
    int n_children = 0;
    int this_dragged = 0;
    int container_redraw = 0;
    
    /* if there are no children for this container */
    if (!container->object) {
        return;
    }
    
    /* see if this container holds possible drag */
    if (EUI_RECT_HAS_COORD(&container->rc, view->context->touch.drag_start_x, view->context->touch.drag_start_y)
        && view->context->touch.state_cache == EUI_FLAG_TOUCH_DRAGGED) {
        this_dragged = 1;
    }
    
    /* calculate width and height of fixed size children */
    for (child = container->object; child; child = child->next) {
        if (child->w_fixed > 0) {
            total_w += child->w_fixed;
            n_children_fixed_x++;
        }
        if (child->h_fixed > 0) {
            total_h += child->h_fixed;
            n_children_fixed_y++;
        }
        n_children++;
    }
    
    /* resolve width of dynamically resized children */
    if (container->child_w > 0) {
        /* fixed size set */
        child_w = container->child_w;
    } else if (n_children > n_children_fixed_x) {
        /* dynamic size calculated by size of the container */
        if (EUI_RECT_W(&container->rc) > total_w) {
            child_w = (EUI_RECT_W(&container->rc) - total_w) / (n_children - n_children_fixed_x);
        } else {
            child_w = 0;
        }
    }
    
    /* resolve height of dynamically resized children */
    if (container->child_h > 0) {
        /* fixed size set */
        child_h = container->child_h;
    } else if (n_children > n_children_fixed_y) {
        /* dynamic size calculated by size of the container */
        if (EUI_RECT_H(&container->rc) > total_h) {
            child_h = (EUI_RECT_H(&container->rc) - total_h) / (n_children - n_children_fixed_y);
        } else {
            child_h = 0;
        }
    }
    
    if (view->context->touch.state == EUI_FLAG_TOUCH_DRAG_RELEASED) {
        container->offset_x = container->offset_x_temp;
        container->offset_y = container->offset_y_temp;
    }
    
    /* calculate base position and size of children in container */
    total_h = 0;
    total_w = 0;
    child_x = container->rc.x1;
    child_y = container->rc.y1;
    if (container->type == EUI_OBJECT_AREA || container->type == EUI_OBJECT_HBOX) {
        child_x = EUI_COORD_NONE;
        child_h = EUI_RECT_H(&container->rc);
    }
    if (container->type == EUI_OBJECT_AREA || container->type == EUI_OBJECT_VBOX) {
        child_y = EUI_COORD_NONE;
        child_w = EUI_RECT_W(&container->rc);
    }
    eui_base_calculate(view, container,
                       child_x, child_y,
                       child_w, child_h,
                       &total_w, &total_h,
                       this_dragged);
    
    if (this_dragged) {
        x = container->rc.x1 + view->context->touch.drag_x + container->offset_x;
        y = container->rc.y1 + view->context->touch.drag_y + container->offset_y;
        
        if (x > container->rc.x1) {
            x = container->rc.x1;
        } else if ((x + total_w - 1) < container->rc.x2) {
            x = container->rc.x2 - total_w + 1;
            container_redraw = 1;
        }
        
        if (y > container->rc.y1) {
            y = container->rc.y1;
        } else if ((y + total_h - 1) < container->rc.y2) {
            y = container->rc.y2 - total_h + 1;
            container_redraw = 1;
        }
        
        container->offset_x_temp = x - container->rc.x1;
        container->offset_y_temp = y - container->rc.y1;
    } else {
        x = container->rc.x1 + container->offset_x;
        y = container->rc.y1 + container->offset_y;
    }
    
    /* add (drag) offset to children */
    if (container->type == EUI_OBJECT_AREA || container->type == EUI_OBJECT_HBOX) {
        for (child = container->object; child; child = child->next) {
            child->rc.x1 += x;
            child->rc.x2 += x;
            if (container_redraw && this_dragged) {
                EUI_FLAG_ON(child, EUI_FLAG_REDRAW);
            }
        }
    }
    if (container->type == EUI_OBJECT_AREA || container->type == EUI_OBJECT_VBOX) {
        for (child = container->object; child; child = child->next) {
            child->rc.y1 += y;
            child->rc.y2 += y;
            if (container_redraw && this_dragged) {
                EUI_FLAG_ON(child, EUI_FLAG_REDRAW);
            }
        }
    }
}


