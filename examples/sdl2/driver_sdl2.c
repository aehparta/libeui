/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "driver_sdl2.h"


/******************************************************************************/
int eui_driver_init(eui_context_t *context)
{
    return 0;
}


/******************************************************************************/
void eui_clear_display(eui_context_t *context)
{
    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = context->w;
    rc.h = context->h;
    
    SDL_SetRenderDrawColor(context->display, EUI_COLOR_R(context->bg), EUI_COLOR_G(context->bg), EUI_COLOR_B(context->bg), 255);
    SDL_RenderFillRect(context->display, &rc);
}


/******************************************************************************/
void eui_set_clip_region(eui_context_t *context, eui_rect_t *rect)
{
    SDL_Rect rc;
    rc.x = rect->x1;
    rc.y = rect->y1;
    rc.w = rect->x2 - rect->x1 + 1;
    rc.h = rect->y2 - rect->y1 + 1;
    context->clip = *rect;
    SDL_RenderSetClipRect(context->display, &rc);
}


/******************************************************************************/
void eui_draw_pixel(eui_context_t *context, eui_coord_t x, eui_coord_t y, eui_color_t color)
{
    SDL_SetRenderDrawColor(context->display, EUI_COLOR_R(color), EUI_COLOR_G(color), EUI_COLOR_B(color), 255);
    SDL_RenderDrawPoint(context->display, x, y);
}


/******************************************************************************/
void eui_draw_line_h(eui_context_t *context, eui_coord_t x1, eui_coord_t x2, eui_coord_t y, eui_color_t color)
{
    SDL_SetRenderDrawColor(context->display, EUI_COLOR_R(color), EUI_COLOR_G(color), EUI_COLOR_B(color), 255);
    SDL_RenderDrawLine(context->display, x1, y, x2, y);
}


/******************************************************************************/
void eui_draw_line_v(eui_context_t *context, eui_coord_t y1, eui_coord_t y2, eui_coord_t x, eui_color_t color)
{
    SDL_SetRenderDrawColor(context->display, EUI_COLOR_R(color), EUI_COLOR_G(color), EUI_COLOR_B(color), 255);
    SDL_RenderDrawLine(context->display, x, y1, x, y2);
}


/******************************************************************************/
void eui_draw_rect(eui_context_t *context, eui_rect_t *rect)
{
    SDL_Point points[5];
    
    SDL_SetRenderDrawColor(context->display, EUI_COLOR_R(context->fg), EUI_COLOR_G(context->fg), EUI_COLOR_B(context->fg), 255);
    
    points[0].x = rect->x1;
    points[0].y = rect->y1;
    points[1].x = rect->x2;
    points[1].y = rect->y1;
    points[2].x = rect->x2;
    points[2].y = rect->y2;
    points[3].x = rect->x1;
    points[3].y = rect->y2;
    points[4].x = rect->x1;
    points[4].y = rect->y1;
    
    SDL_RenderDrawLines(context->display, points, 5);
}


/******************************************************************************/
void eui_fill_rect(eui_context_t *context, eui_rect_t *rect)
{
    SDL_Rect rc;
    rc.x = rect->x1;
    rc.y = rect->y1;
    rc.w = rect->x2 - rect->x1 + 1;
    rc.h = rect->y2 - rect->y1 + 1;
    
    SDL_SetRenderDrawColor(context->display, EUI_COLOR_R(context->fg), EUI_COLOR_G(context->fg), EUI_COLOR_B(context->fg), 255);
    SDL_RenderFillRect(context->display, &rc);
}


