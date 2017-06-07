 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef MAIN_H
#define MAIN_H

/******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif
#include "driver_sdl2.h"

extern const eui_font_t g_sFontFixed6x8;
extern const eui_font_t g_sFontCm16b;
extern const eui_font_t g_sFontCm30;


/******************************************************************************/
int main(int argc, char *argv[]);


#endif /* MAIN_H - END OF HEADER FILE */
