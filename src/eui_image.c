/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_image.h"


/******************************************************************************/
void eui_image_init(eui_image_t *image, const eui_image_data_t *data)
{
    memset(image, 0, sizeof(*image));
    eui_object_init((eui_object_t *)image);
    
    image->type = EUI_OBJECT_IMAGE;
    image->data = data;
    image->w_fixed = data->w;
    image->h_fixed = data->h;
}


/******************************************************************************/
void eui_image_draw(eui_view_t *view, eui_image_t *image)
{
    uint8_t *data, byte, count, bit, bpp, r, g, b;
    uint8_t rle_boundary;
    eui_coord_t x, y;
    eui_color_t colors[16];
    
    data = (uint8_t *)image->data->data;
    count = 0;
    x = image->rc.x1;
    y = image->rc.y1;
    
    //EUI_DEBUG_PRINT_RECT(image->rc);
    
    bpp = image->data->format & EUI_IMAGE_FORMAT_BPP_MASK;
    
    switch (image->data->format & EUI_IMAGE_FORMAT_RLE_MASK) {
    case EUI_IMAGE_FORMAT_RLE2:
        rle_boundary = 0xfc;
        break;
    case EUI_IMAGE_FORMAT_RLE3:
        rle_boundary = 0xf8;
        break;
    case EUI_IMAGE_FORMAT_RLE4:
        rle_boundary = 0xf0;
        break;
    case EUI_IMAGE_FORMAT_RLE5:
        rle_boundary = 0xe0;
        break;
    case EUI_IMAGE_FORMAT_RLE6:
        rle_boundary = 0xc0;
        break;
    case EUI_IMAGE_FORMAT_RLE7:
        rle_boundary = 0x80;
        break;
    default:
        rle_boundary = 0xff;
        break;
    }
    
    if (bpp < 8) {
        colors[0] = view->fg;
        colors[15] = view->bg;
        if (image->fg > -1) {
            colors[0] = image->fg;
        }
        if (image->bg > -1) {
            colors[15] = image->bg;
        }
        
        if (EUI_FLAG_IS(image, EUI_FLAG_INVERSE)) {
            colors[1] = colors[0];
            colors[0] = colors[15];
            colors[15] = colors[1];
        }
        
        if (bpp == 2) {
            /* rgb values */
            int r, g, b;
            /* deltas */
            int rd, gd, bd;
            
            colors[3] = colors[15];
            
            r = (int)EUI_COLOR_R(colors[0]);
            g = (int)EUI_COLOR_G(colors[0]);
            b = (int)EUI_COLOR_B(colors[0]);
            rd = ((int)EUI_COLOR_R(colors[3]) - r) / 3;
            gd = ((int)EUI_COLOR_G(colors[3]) - g) / 3;
            bd = ((int)EUI_COLOR_B(colors[3]) - b) / 3;
            
            colors[1] = (eui_color_t)EUI_COLOR_RGB(r + rd, g + gd, b + bd);
            colors[2] = (eui_color_t)EUI_COLOR_RGB(r + (rd << 1), g + (gd << 1), b + (bd << 1));
        } else if (bpp == 4) {
            /* rgb values */
            int r, g, b;
            /* deltas */
            int rd, gd, bd;
            
            r = (int)EUI_COLOR_R(colors[0]);
            g = (int)EUI_COLOR_G(colors[0]);
            b = (int)EUI_COLOR_B(colors[0]);
            rd = ((int)EUI_COLOR_R(colors[15]) - r) / 15;
            gd = ((int)EUI_COLOR_G(colors[15]) - g) / 15;
            bd = ((int)EUI_COLOR_B(colors[15]) - b) / 15;
            
            r += rd; g += gd; b += bd;
            colors[1] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[2] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[3] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[4] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[5] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[6] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[7] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[8] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[9] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[10] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[11] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[12] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[13] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
            r += rd; g += gd; b += bd;
            colors[14] = (eui_color_t)EUI_COLOR_RGB(r, g, b);
        }
    }
    
    for ( ; y <= image->rc.y2; data++) {
        byte = *data;
        if (count == 0) {
            if (byte > rle_boundary) {
                count = byte & ~rle_boundary;
                continue;
            }
            count = 1;
        }
        
        for ( ; count > 0; count--) {
            switch (bpp) {
            case 1:
                for (bit = 0x80; bit > 0 && y <= image->rc.y2; bit = bit >> 1) {
                    if (byte & bit) {
                        eui_draw_pixel(view->context, x, y, colors[15]);
                    } else if (EUI_FLAG_IS(image, EUI_FLAG_OPAQUE)) {
                        eui_draw_pixel(view->context, x, y, colors[0]);
                    }
                    x++;
                    if (x > image->rc.x2) {
                        y++;
                        x = image->rc.x1;
                    }
                }
                break;
                
            case 2:
                for (bit = 0xc0; bit > 0 && y <= image->rc.y2; bit = bit >> 2) {
                    switch (byte & bit) {
                    case 0xc0:
                    case 0x30:
                    case 0x0c:
                    case 0x03:
                        eui_draw_pixel(view->context, x, y, colors[3]);
                        break;
                    case 0x80:
                    case 0x20:
                    case 0x08:
                    case 0x02:
                        eui_draw_pixel(view->context, x, y, colors[2]);
                        break;
                    case 0x40:
                    case 0x10:
                    case 0x04:
                    case 0x01:
                        eui_draw_pixel(view->context, x, y, colors[1]);
                        break;
                    case 0x00:
                        eui_draw_pixel(view->context, x, y, colors[0]);
                        break;
                    }
                    x++;
                    if (x > image->rc.x2) {
                        y++;
                        x = image->rc.x1;
                    }
                }
                break;
                
            case 4:
                for (bit = 0xf0; bit > 0 && y <= image->rc.y2; bit = bit >> 4) {
                    r = ((bit > 0x0f) ? (byte >> 4) : byte) & 0x0f;
                    eui_draw_pixel(view->context, x, y, colors[r]);
                    x++;
                    if (x > image->rc.x2) {
                        y++;
                        x = image->rc.x1;
                    }
                }
                break;
                
            case 8:
                r = ((byte & 0xe0) >> 5) * 36; /* 3-bits */
                g = ((byte & 0x1c) >> 2) * 36; /* 3-bits */
                b = (byte & 0x03) * 85; /* 2-bits */
                eui_draw_pixel(view->context, x, y, EUI_COLOR_RGB(r, g, b));
                x++;
                if (x > image->rc.x2) {
                    y++;
                    x = image->rc.x1;
                }
                break;
            }
        }
    }
    printf("redraw\n");
//     uint8_t mask;
//     
//     for (y = 0; y < 32; y++) {
//         uint16_t row = y * 4;
//         for (x = 0; x < 32; x++) {
//             mask = (uint8_t)(8 >> (x & 0x0007));
//             if (image->data->data[(x >> 3) | row] & mask) {
//                 eui_draw_pixel(view->context, x, y, EUI_COLOR_BLACK);
//             } else {
//                 eui_draw_pixel(view->context, x, y, EUI_COLOR_WHITE);
//             }
//         }
//     }
}
