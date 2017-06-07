 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUILIB_DRIVER_MSP_GRLIB_H
#define EUILIB_DRIVER_MSP_GRLIB_H


/*
 * definitions for msp43x target using TI MSP GRLIB
 * http://www.ti.com/tool/msp-grlib
 */
#ifdef LIBEUI_DRIVER_MSP_GRLIB


/******************************************************************************/
#include <grlib.h>


/* basic type definitions */
#define eui_color_t int16_t
#define eui_coord_t int16_t

/* rectangle */
typedef struct
{
    eui_coord_t x1;
    eui_coord_t y1;
    eui_coord_t x2;
    eui_coord_t y2;
} eui_rect_t;

/* use grlib font definitions */
#define eui_font_t      Graphics_Font
#define eui_font_ex_t   Graphics_FontEx

/* touch sensing */
#define EUI_TOUCH_DELAY 30
#define EUI_TOUCH_BUFFER 5
#define EUI_TOUCH_DRAG_THRESHOLD 5
#define EUI_TOUCH_KINETIC_THRESHOLD 3

typedef struct
{
    uint8_t state;
    uint8_t state_cache;

    eui_coord_t x;
    eui_coord_t x_cache;
    eui_coord_t y;
    eui_coord_t y_cache;
    
    eui_coord_t buf_x[EUI_TOUCH_BUFFER];
    eui_coord_t buf_y[EUI_TOUCH_BUFFER];
    uint8_t buf_c;
    
    eui_coord_t drag_x;
    eui_coord_t drag_y;
    float drag_start_x;
    float drag_start_y;
    
    uint8_t delay;
    int8_t speed_x;
    int8_t speed_y;
    
    uint8_t kinetic_motion;
} eui_touch_t;


/* main graphics driver context */
typedef struct
{
    Graphics_Context display;
    eui_color_t fg;
    eui_color_t bg;
    
    eui_coord_t w;
    eui_coord_t h;
    
    eui_touch_t touch;
    eui_rect_t clip;
    const eui_font_t *font;
} eui_context_t;

/* initialization */
#define eui_driver_init(context) 

/* generic functions */
#define eui_clear_display(context) GrClearDisplay(&(context)->display)
#define eui_set_clip_region(context, rect) do { (context)->clip = *(rect); GrContextClipRegionSet(&(context)->display, (Graphics_Rectangle *)rect); } while(0)

/* color functions */
#define eui_set_fg(context, color) do { (context)->fg = (color); GrContextForegroundSet(&(context)->display, EUI_COLOR_16TO24(color)); } while(0)
#define eui_set_bg(context, color) do { (context)->bg = (color); GrContextBackgroundSet(&(context)->display, EUI_COLOR_16TO24(color)); } while(0)

// #define eui_draw_pixel(context, x, y, color) do { (context)->display.display->callPixelDraw((context)->display.display->displayData, x, y, EUI_COLOR_16TO24(color)); } while(0)
// #define eui_draw_line_h(context, x1, x2, y, color) do { (context)->display.display->callLineDrawH((context)->display.display->displayData, x1, x2, y, EUI_COLOR_16TO24(color)); } while(0)
// #define eui_draw_line_v(context, y1, y2, x, color) do { (context)->display.display->callLineDrawV((context)->display.display->displayData, x, y1, y2, EUI_COLOR_16TO24(color)); } while(0)
// #define eui_draw_rect(context, rect) Graphics_drawRectangle(&(context)->display, rect)

/* pixel/line functions */
#define eui_draw_pixel(context, x, y, color) do { eui_set_fg(context, color); Graphics_drawPixel(&(context)->display, x, y); } while(0)
#define eui_draw_line_h(context, x1, x2, y, color) do { eui_set_fg(context, color); Graphics_drawLineH(&(context)->display, x1, x2, y); } while(0)
#define eui_draw_line_v(context, x, y1, y2, color) do { eui_set_fg(context, color); Graphics_drawLineV(&(context)->display, x, y1, y2); } while(0)

/* rectangle functions */
#define eui_draw_rect(context, rect) GrRectDraw(&(context)->display, (Graphics_Rectangle *)rect)
#define eui_fill_rect(context, rect) GrRectFill(&(context)->display, (Graphics_Rectangle *)rect)
//#define eui_fill_rect(context, rect) (context)->display.display->callRectFill((context)->display.display->displayData, rect, (context)->fg)

/* circle functions */
#define eui_draw_circle(context, x, y, radius) GrCircleDraw(&(context)->display, x, y, radius);
#define eui_fill_circle(context, x, y, radius) GrCircleFill(&(context)->display, x, y, radius);


#endif /* LIBEUI_DRIVER_MSP_GRLIB */

#endif /* EUILIB_DRIVER_MSP_GRLIB_H - END OF HEADER FILE */

