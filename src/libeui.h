 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUILIB_H
#define EUILIB_H

#ifdef _WIN32
#include <stdio.h>
#endif

/* color conversion macros */
#define EUI_COLOR_16TO24(color) ((((color) & 0x7c00) << 9) | (((color) & 0x03e0) << 6) | (((color) & 0x001f) << 3))
#define EUI_COLOR_24TO16(color) ((((color) & 0xf80000) >> 9) | (((color) & 0xf800) >> 6) | (((color) & 0xf8) >> 3)) 
#define EUI_COLOR_RGB(r, g, b) ((((r) & 0xf8) << 7) | (((g) & 0xf8) << 2) | (((b) & 0xf8) >> 3))
#define EUI_COLOR_R(color) (((color) & 0x7c00) >> 7)
#define EUI_COLOR_G(color) (((color) & 0x3e0) >> 2)
#define EUI_COLOR_B(color) (((color) & 0x1f) << 3)

#include "eui_common.h"


#define EUI_COORD_NONE                  0x7fff

/* colors */
#define EUI_COLOR_BLACK                 0x0000
#define EUI_COLOR_WHITE                 0x7fff

/* flag bits */
#define EUI_FLAG_INVERSE                0x01
#define EUI_FLAG_OPAQUE                 0x02
#define EUI_FLAG_REDRAW                 0x04

#define EUI_FLAG_TOUCH_MASK             0xe0
#define EUI_FLAG_TOUCH_NONE             0x00
#define EUI_FLAG_TOUCH_PRESSED          0x20
#define EUI_FLAG_TOUCH_RELEASED         0x40
#define EUI_FLAG_TOUCH_DRAGGED          0x60
#define EUI_FLAG_TOUCH_DRAG_RELEASED    0x80

#define EUI_FLAG_ON(object, flag)    ((object)->flags |= flag)
#define EUI_FLAG_OFF(object, flag)   ((object)->flags &= ~flag)
#define EUI_FLAG_IS(object, flag)    ((object)->flags & flag)

#define EUI_FLAG_TOUCH_SET(object, state) \
    object->flags = (object->flags & ~EUI_FLAG_TOUCH_MASK) | state
#define EUI_FLAG_TOUCH_GET(object) (object->flags & EUI_FLAG_TOUCH_MASK)
#define EUI_FLAG_TOUCH_IS(object, state) \
    ((object->flags & EUI_FLAG_TOUCH_MASK) == state)


#define EUI_OBJECT_HEADER \
    void *next; \
    uint8_t type; \
    uint8_t flags; \
    eui_color_t fg; \
    eui_color_t bg; \
    eui_rect_t rc; \
    eui_coord_t w_fixed; \
    eui_coord_t h_fixed; \
    uint8_t pad_outter; \
    uint8_t border;

typedef struct {
    EUI_OBJECT_HEADER
} eui_object_t;


#define EUI_CONTAINER_HEADER \
    void *next; \
    uint8_t type; \
    uint8_t flags; \
    eui_color_t fg; \
    eui_color_t bg; \
    eui_rect_t rc; \
    eui_coord_t w_fixed; \
    eui_coord_t h_fixed; \
    uint8_t pad_outter; \
    uint8_t border; \
    eui_object_t *object; \
    eui_coord_t child_w; \
    eui_coord_t child_h; \
    eui_coord_t offset_x; \
    eui_coord_t offset_x_temp; \
    eui_coord_t offset_y; \
    eui_coord_t offset_y_temp;

typedef struct {
    EUI_CONTAINER_HEADER
} eui_container_t;

typedef struct {
    EUI_CONTAINER_HEADER
} eui_area_t;

typedef struct {
    EUI_CONTAINER_HEADER
} eui_box_t;


typedef struct {
    eui_rect_t rc;
    eui_color_t fg;
    eui_color_t bg;
    const eui_font_t *font;
    
    eui_object_t *object;
    
    eui_context_t *context;
} eui_view_t;


/******************************************************************************/
/* images */
typedef struct {
    uint16_t w;
    uint16_t h;
    uint8_t format; /* format of this image */
    uint8_t data[];
} eui_image_data_t;

#define EUI_IMAGE_FORMAT_BPP_MASK       0x1f
#define EUI_IMAGE_FORMAT_1BIT           1
#define EUI_IMAGE_FORMAT_2BIT           2
#define EUI_IMAGE_FORMAT_4BIT           4
#define EUI_IMAGE_FORMAT_8BIT           8
#define EUI_IMAGE_FORMAT_16BIT          16
#define EUI_IMAGE_FORMAT_24BIT          24
#define EUI_IMAGE_FORMAT_32BIT          32

#define EUI_IMAGE_FORMAT_RLE_MASK       0xe0
#define EUI_IMAGE_FORMAT_RLE2           0x20
#define EUI_IMAGE_FORMAT_RLE3           0x40
#define EUI_IMAGE_FORMAT_RLE4           0x60
#define EUI_IMAGE_FORMAT_RLE5           0x80
#define EUI_IMAGE_FORMAT_RLE6           0xa0
#define EUI_IMAGE_FORMAT_RLE7           0xc0


/******************************************************************************/
enum {
    /* normal objects */
    EUI_OBJECT_BUTTON = 0,
    EUI_OBJECT_LABEL,
    EUI_OBJECT_IMAGE,
    /* containers */
    EUI_OBJECT_CONTAINERS,
    EUI_OBJECT_VBOX,
    EUI_OBJECT_HBOX,
    EUI_OBJECT_AREA,
};


/******************************************************************************/
/* some macros for convenience */
#define EUI_RECT_W(rc)                  ((rc)->x2 - (rc)->x1 + 1)
#define EUI_RECT_H(rc)                  ((rc)->y2 - (rc)->y1 + 1)
#define EUI_RECT_HAS_COORD(rc, x, y)    ((rc)->x1 <= (x) && (rc)->x2 >= (x) && \
                                         (rc)->y1 <= (y) && (rc)->y2 >= (y))
#define EUI_RECTS_INTERSECT(rc1, rc2)   ((rc1)->x1 <= (rc2)->x2 && (rc1)->x2 >= (rc2)->x1 && \
                                         (rc1)->y1 <= (rc2)->y2 && (rc1)->y2 >= (rc2)->y1)
                                                                            
/******************************************************************************/
/* for debug */
#define EUI_DEBUG_PRINT_RECT(rc) printf("x1: %4d, y1: %4d, x2: %4d, y2: %4d\n", rc.x1, rc.y1, rc.x2, rc.y2)


/******************************************************************************/
int eui_init(eui_context_t *context, eui_coord_t screen_w, eui_coord_t screen_h);


/******************************************************************************/
#include "eui_object.h"
#include "eui_label.h"
#include "eui_button.h"
#include "eui_view.h"
#include "eui_container.h"
#include "eui_string.h"

#include "eui_status.h"
#include "eui_calibrate.h"
#include "eui_touch.h"

#include "eui_image.h"


#endif /* EUILIB_H - END OF HEADER FILE */
