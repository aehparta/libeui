/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
int eui_init(eui_context_t *context, eui_coord_t screen_w, eui_coord_t screen_h)
{
    /* initialize display */
//    SSD1289_DriverInit();
    /* clear screen */
//    Graphics_initContext(&eui_context, &g_sSSD1289_Driver);
    /* set screen size */
//    eui_screen_w = LCD_X_SIZE;
//    eui_screen_h = LCD_Y_SIZE;
    
    context->w = screen_w;
    context->h = screen_h;
    eui_set_fg(context, EUI_COLOR_BLACK);
    eui_set_bg(context, EUI_COLOR_WHITE);
    eui_clear_display(context);
    
    return 0;
}


