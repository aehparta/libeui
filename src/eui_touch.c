/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_touch.h"


/******************************************************************************/
void eui_touch_reset(eui_context_t *context)
{
    int i, j;
    float x = 0, y = 0;
    
    /* reset all touch related variables */
    for (i = 0, j = 0; i < (EUI_TOUCH_BUFFER - 1); i++) {
        if (context->touch.buf_x[context->touch.buf_c] > -1 && context->touch.buf_y[context->touch.buf_c] > -1) {
            x += (float)context->touch.buf_x[context->touch.buf_c];
            y += (float)context->touch.buf_y[context->touch.buf_c];
            j++;
        }
        context->touch.buf_c++;
        if (context->touch.buf_c >= EUI_TOUCH_BUFFER) {
            context->touch.buf_c = 0;
        }
    }
    for (i = 0; i < EUI_TOUCH_BUFFER; i++) {
        context->touch.buf_x[i] = -1;
        context->touch.buf_y[i] = -1;
    }
    context->touch.buf_c = 0;
    context->touch.drag_start_x = -1;
    context->touch.drag_start_y = -1;
    context->touch.drag_x = 0;
    context->touch.drag_y = 0;
    context->touch.x_cache = (eui_coord_t)(x / j);
    context->touch.y_cache = (eui_coord_t)(y / j);
    context->touch.delay = 0;
    context->touch.speed_x = 0;
    context->touch.speed_y = 0;
    context->touch.kinetic_motion = 0;
}


/******************************************************************************/
void eui_touch_init(eui_context_t *context)
{
    eui_touch_reset(context);
    context->touch.state = EUI_FLAG_TOUCH_NONE;
    context->touch.state_cache = EUI_FLAG_TOUCH_NONE;
    context->touch.x = -1;
    context->touch.x_cache = -1;
    context->touch.y = -1;
    context->touch.y_cache = -1;
}


/******************************************************************************/
void eui_touch_update(eui_context_t *context, eui_coord_t x, eui_coord_t y, int pressed)
{
    if (context->touch.delay > 0) {
        /* if screen was touched and released,
         * make some delay before accepting again
         */
        context->touch.delay--;
    } else if (context->touch.state_cache == EUI_FLAG_TOUCH_DRAGGED && !pressed) {
        if (abs(context->touch.speed_x) > EUI_TOUCH_KINETIC_THRESHOLD || abs(context->touch.speed_y) > EUI_TOUCH_KINETIC_THRESHOLD) {
            context->touch.kinetic_motion = 1;
            context->touch.drag_x += (context->touch.speed_x >> 3);
            context->touch.drag_y += (context->touch.speed_y >> 3);
            if (context->touch.speed_x > 0) {
                context->touch.speed_x--;
            } else {
                context->touch.speed_x++;
            }
            if (context->touch.speed_y > 0) {
                context->touch.speed_y--;
            } else {
                context->touch.speed_y++;
            }
        } else {
            eui_touch_reset(context);
            context->touch.state_cache = EUI_FLAG_TOUCH_DRAG_RELEASED;
            context->touch.delay = EUI_TOUCH_DELAY;
        }
    } else if (pressed && context->touch.kinetic_motion) {
        eui_touch_reset(context);
        context->touch.state_cache = EUI_FLAG_TOUCH_DRAG_RELEASED;
    } else if (pressed) {
        /* if screen is being touched */
        if (context->touch.state_cache != EUI_FLAG_TOUCH_DRAGGED) {
            context->touch.state_cache = EUI_FLAG_TOUCH_PRESSED;
        }
        
        context->touch.x_cache = x;
        context->touch.y_cache = y;
        
        context->touch.buf_x[context->touch.buf_c] = context->touch.x_cache;
        context->touch.buf_y[context->touch.buf_c] = context->touch.y_cache;
        context->touch.buf_c++;
        if (context->touch.buf_c >= EUI_TOUCH_BUFFER) {
            int i;
            float x = 0, y = 0;
            
            context->touch.buf_c = 0;
            
            /* check if user is trying to drag */
            for (i = 1; i < EUI_TOUCH_BUFFER; i++) {
                x += (float)context->touch.buf_x[i];
                y += (float)context->touch.buf_y[i];
            }
            x = x / (EUI_TOUCH_BUFFER - 1);
            y = y / (EUI_TOUCH_BUFFER - 1);
            
            if (context->touch.drag_start_x < 0) {
                /* save average of first touch for possible drag */
                context->touch.drag_start_x = x;
                context->touch.drag_start_y = y;
            } else {
                /* now for the actual drag detection */
                if (abs((int)context->touch.drag_start_x - (int)x) > EUI_TOUCH_DRAG_THRESHOLD) {
                    context->touch.state_cache = EUI_FLAG_TOUCH_DRAGGED;
                }
                if (abs((int)context->touch.drag_start_y - (int)y) > EUI_TOUCH_DRAG_THRESHOLD) {
                    context->touch.state_cache = EUI_FLAG_TOUCH_DRAGGED;
                }
            }
            if (context->touch.state_cache == EUI_FLAG_TOUCH_DRAGGED) {
                eui_coord_t last_x = context->touch.drag_x;
                eui_coord_t last_y = context->touch.drag_y;
                context->touch.drag_x = (int)x - (int)context->touch.drag_start_x;
                context->touch.drag_y = (int)y - (int)context->touch.drag_start_y;
                context->touch.speed_x = (context->touch.drag_x - last_x) << 1;
                context->touch.speed_y = (context->touch.drag_y - last_y) << 1;
            }
        }
    } else if (context->touch.state_cache == EUI_FLAG_TOUCH_PRESSED) {
        eui_touch_reset(context);
        context->touch.state_cache = EUI_FLAG_TOUCH_RELEASED;
        context->touch.delay = EUI_TOUCH_DELAY;
    }
}


/******************************************************************************/
void eui_touch_read(eui_context_t *context)
{
    context->touch.x = context->touch.x_cache;
    context->touch.y = context->touch.y_cache;
    context->touch.state = context->touch.state_cache;
    
    if (context->touch.state_cache == EUI_FLAG_TOUCH_RELEASED) {
        context->touch.state_cache = EUI_FLAG_TOUCH_NONE;
    }
    if (context->touch.state_cache == EUI_FLAG_TOUCH_DRAG_RELEASED) {
        context->touch.state_cache = EUI_FLAG_TOUCH_NONE;
    }
}

