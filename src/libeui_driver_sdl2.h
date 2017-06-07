 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUILIB_DRIVER_SDL2_H
#define EUILIB_DRIVER_SDL2_H


/*
 * definitions for SDL2 driver
 */
#ifdef LIBEUI_DRIVER_SDL2


/******************************************************************************/
#ifdef _WIN32
#include <stdio.h>
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

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


/******************************************************************************/
/**
 * This structure describes a font used for drawing text onto the screen.
 */
typedef struct
{
    uint8_t format;         //!< The format of the font.  Can be one of FONT_FMT_UNCOMPRESSED or FONT_FMT_PIXEL_RLE.
    uint8_t maxWidth;       //!< The maximum width of a character; this is the width of the widest character in the font, though any individual character may be narrower than this width.
    uint8_t height;         //!< The height of the character cell; this may be taller than the font data for the characters (to provide inter-line spacing).
    uint8_t baseline;       //!< The offset between the top of the character cell and the baseline of  the glyph.  The baseline is the bottom row of a capital letter, below which only the descenders of the lower case letters occur.
    uint16_t offset[96];    //!< The offset within data to the data for each character in the font.
    const uint8_t *data;    //!< A pointer to the data for the font.
} eui_font_t;

/******************************************************************************/
/**
 * This is a newer version of the structure which describes a font used
 * for drawing text onto the screen.  This variant allows a font to contain an
 * arbitrary, contiguous block of codepoints from the 256 basic characters in
 * an ISO8859-n font and allows support for accented characters in Western
 * European languages and any left-to-right typeface supported by an ISO8859
 * variant. Fonts encoded in this format may be used interchangeably with the
 * original fonts merely by casting the structure pointer when calling any
 * function or macro which expects a font pointer as a parameter.
 */
typedef struct
{
    uint8_t format;			//!< The format of the font.  Can be one of FONT_FMT_EX_UNCOMPRESSED or FONT_FMT_EX_PIXEL_RLE.
    uint8_t maxWidth;		//!< The maximum width of a character; this is the width of the widest character in the font, though any individual character may be narrower than this width.
    uint8_t height;			//!< The height of the character cell; this may be taller than the font data for the characters (to provide inter-line spacing).
    uint8_t baseline;		//!< The offset between the top of the character cell and the baseline of the glyph.  The baseline is the bottom row of a capital letter, below which only the descenders of the lower case letters occur.
    uint8_t first;		    //!< The codepoint number representing the first character encoded in the font.
    uint8_t last;			//!< The codepoint number representing the last character encoded in the font.
    const uint16_t *offset;	//!< A pointer to a table containing the offset within data to the data for each character in the font.
    const uint8_t *data;	//!< A pointer to the data for the font.
} eui_font_ex_t;


/* touch sensing */
#define EUI_TOUCH_DELAY 10
#define EUI_TOUCH_BUFFER 10
#define EUI_TOUCH_DRAG_THRESHOLD 3
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
    void *display;
    eui_color_t fg;
    eui_color_t bg;
    
    eui_coord_t w;
    eui_coord_t h;
    
    eui_touch_t touch;
    eui_rect_t clip;
    const eui_font_t *font;
} eui_context_t;

/* initialization */
int eui_driver_init(eui_context_t *context);

/* generic functions */
void eui_clear_display(eui_context_t *context);
void eui_set_clip_region(eui_context_t *context, eui_rect_t *rect);

/* pixel/line functions */
void eui_draw_pixel(eui_context_t *context, eui_coord_t x, eui_coord_t y, eui_color_t color);
void eui_draw_line_h(eui_context_t *context, eui_coord_t x1, eui_coord_t x2, eui_coord_t y, eui_color_t color);
void eui_draw_line_v(eui_context_t *context, eui_coord_t y1, eui_coord_t y2, eui_coord_t x, eui_color_t color);

/* color functions */
#define eui_set_fg(context, color) do { (context)->fg = color; } while(0)
#define eui_set_bg(context, color) do { (context)->bg = color; } while(0)

/* rectangle functions */
void eui_draw_rect(eui_context_t *context, eui_rect_t *rect);
void eui_fill_rect(eui_context_t *context, eui_rect_t *rect);


#endif /* LIBEUI_DRIVER_MSP_GRLIB */

#endif /* EUILIB_DRIVER_MSP_GRLIB_H - END OF HEADER FILE */

