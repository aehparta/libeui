/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_status.h"


/******************************************************************************/
int ui_status_is_on = 0;
ui_coord_t ui_status_height = 14;
static char ui_status_string[32];
static int ui_status_bat_level = -1;


/******************************************************************************/
void eui_status_init(eui_context_t *context)
{
    memset(ui_status_string, 0, sizeof(ui_status_string));
}


/******************************************************************************/
void eui_status_set(const char *status)
{
    strncpy(ui_status_string, status, sizeof(ui_status_string) - 1);
    ui_status_is_on = 2;
}


/******************************************************************************/
void eui_status_on()
{
    ui_status_is_on = 2;
}


/******************************************************************************/
void eui_status_off()
{
    ui_status_is_on = 0;
}


/******************************************************************************/
void eui_status_rtc_to_str(char *str)
{
    *(str++) = (RTCDAY >> 4) + '0';
    *(str++) = (RTCDAY & 0xf) + '0';
    *(str++) = '.';
    *(str++) = (RTCMON >> 4) + '0';
    *(str++) = (RTCMON & 0xf) + '0';
    *(str++) = '.';
    *(str++) = (RTCYEAR >> 12) + '0';
    *(str++) = ((RTCYEAR >> 8) & 0xf) + '0';
    *(str++) = ((RTCYEAR >> 4) & 0xf) + '0';
    *(str++) = (RTCYEAR & 0xf) + '0';
    *(str++) = ' ';
    *(str++) = (RTCHOUR >> 4) + '0';
    *(str++) = (RTCHOUR & 0xf) + '0';
    *(str++) = ':';
    *(str++) = (RTCMIN >> 4) + '0';
    *(str++) = (RTCMIN & 0xf) + '0';
    *(str++) = ':';
    *(str++) = (RTCSEC >> 4) + '0';
    *(str++) = (RTCSEC & 0xf) + '0';
    
    *(str) = '\0';
}


/******************************************************************************/
void eui_status_draw()
{
    static int charging_anim_counter = 2;
    char str[32];
    ui_rect_t rc;
    
    if (!ui_status_is_on) {
        return;
    }
    
#define UI_STATUS_BACKGROUND        GRAPHICS_COLOR_PERU

    rc.x1 = 0;
    rc.x2 = LCD_X_SIZE - 1;
    rc.y1 = 0;
    rc.y2 = 13;
    ui_set_clip_region(&ui_context, &rc);
    
    ui_set_bg24(&ui_context, UI_STATUS_BACKGROUND);
    ui_set_font(&ui_context, &g_sFontFixed6x8);
    
    /* do full redraw only if status was just set on or text was changed */
    if (ui_status_is_on == 2) {
        /* fill background */
        rc.x1 = 0;
        rc.x2 = LCD_X_SIZE - 1;
        rc.y1 = 0;
        rc.y2 = 13;
        ui_set_fg24(&ui_context, UI_STATUS_BACKGROUND);
        ui_fill_rect(&ui_context, &rc);

        /* draw status text */
        ui_set_fg24(&ui_context, ClrBlack);
        if (ui_status_bat_level >= 0) {
            ui_draw_str(&ui_context, ui_status_string, -1, 40, 3, 1);
        } else {
            ui_draw_str(&ui_context, ui_status_string, -1, 3, 3, 1);
        }

        ui_status_is_on = 1;
    }
    
    /* draw clock */
    ui_set_fg24(&ui_context, ClrBlack);
    ui_set_bg24(&ui_context, UI_STATUS_BACKGROUND);
    ui_status_rtc_to_str(str);
    ui_draw_str(&ui_context, str, -1, LCD_X_SIZE - 120, 3, 1);
    
    /* draw battery related stuff if battery level is set */
    if (ui_status_bat_level >= 0) {
        /* draw battery icon */
        ui_set_fg24(&ui_context, ClrBlack);
        rc.x1 = 8;
        rc.x2 = 25;
        rc.y1 = 3;
        rc.y2 = 10;
        ui_draw_rect(&ui_context, &rc);
        rc.x1 = 26;
        rc.x2 = 26;
        rc.y1 = 5;
        rc.y2 = 8;
        ui_fill_rect(&ui_context, &rc);
        
        ui_set_fg24(&ui_context, ClrGreen);
        if (ui_status_bat_level < 30) {
            ui_set_fg24(&ui_context, ClrRed);
        } else if (ui_status_bat_level < 70) {
            ui_set_fg24(&ui_context, ClrYellow);
        }
        rc.x1 = 9;
        rc.x2 = 24;
        rc.y1 = 4;
        rc.y2 = 9;
        ui_fill_rect(&ui_context, &rc);
        
        /* when charging, draw animation on battery */
        if (ui_status_bat_level > 100) {
            ui_set_fg24(&ui_context, UI_STATUS_BACKGROUND);
            rc.x1 = 9 + charging_anim_counter;
            rc.x2 = 24;
            rc.y1 = 4;
            rc.y2 = 9;
            if (rc.x1 < 24) {
                ui_fill_rect(&ui_context, &rc);
            }
            
            charging_anim_counter += 2;
            if (charging_anim_counter > 15) {
                charging_anim_counter = 2;
            }
        } else {
            ui_set_fg24(&ui_context, UI_STATUS_BACKGROUND);
            rc.x1 = 9 + (ui_status_bat_level * (25 - 9) / 100);
            rc.x2 = 24;
            rc.y1 = 4;
            rc.y2 = 9;
            if (rc.x1 < 25) {
                ui_fill_rect(&ui_context, &rc);
            }
        }
    }
}


/******************************************************************************/
void eui_status_set_bat_level(int level)
{
    eui_status_bat_level = level;
}

