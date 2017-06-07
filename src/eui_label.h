 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef EUI_LABEL_H
#define EUI_LABEL_H

/******************************************************************************/
#include "libeui.h"


/******************************************************************************/
typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[256];
} eui_label256_t;

typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[128];
} eui_label128_t;

typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[64];
} eui_label64_t;

typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[32];
} eui_label32_t;

typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[16];
} eui_label16_t;

typedef struct {
    EUI_OBJECT_HEADER
    const eui_font_t *font;
    char label[0];
} eui_label_t;


/******************************************************************************/
void eui_label_init(eui_label_t *label, const char *text);
void eui_label_set_text(eui_label_t *label, const char *text);

void eui_label_draw(eui_view_t *view, eui_label_t *label);



#endif /* EUI_LABEL_H - END OF HEADER FILE */
