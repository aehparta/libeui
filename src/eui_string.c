/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include "eui_string.h"


/******************************************************************************/
static int16_t eui_get_number_of_leading_zeros(int32_t x)
{
    int32_t y = 0x80, count = 0;
    int32_t i;
    
    for (i = 0; i < 32; i++) {
        if (0x00 != (x & y )) {
            break;
        }
        count++;
        y = y >> 1;
    }    
    
    return count;
}


/******************************************************************************/
int eui_str_width(const eui_font_t *font, const char *string, int length)
{
    const uint16_t *offset;
    const uint8_t *data;
    uint8_t first, last, absent;
    int32_t  width;

    //
    // Get some pointers to relevant information in the font to make things
    // easier, and give the compiler a hint about extraneous loads that it can
    // avoid.
    //
    if (font->format & EUI_FONT_EX_MARKER) {
        eui_font_ex_t *font_ex = (eui_font_ex_t *)font;

        data = font_ex->data;
        offset = font_ex->offset;
        first = font_ex->first;
        last = font_ex->last;

        //
        // Does the default absent character replacement exist in the font?
        //
        if ((EUI_ABSENT_CHAR_REPLACEMENT >= first) &&
            (EUI_ABSENT_CHAR_REPLACEMENT <= last)) {
            //
            // Yes - use the standard character when an absent character is
            // found.
            //
            absent = EUI_ABSENT_CHAR_REPLACEMENT;
        } else {
            //
            // The default absent character is not present in the font so use
            // the first character (we only use its width here) instead.
            //
            absent = font_ex->first;
        }
    } else {
        data = font->data;
        offset = font->offset;
        first = 32;
        last = 126;
        absent = EUI_ABSENT_CHAR_REPLACEMENT;
    }

    //
    // Loop through the characters in the string.
    //
    for (width = 0; *string && length; string++, length--) {
        //
        // Get a pointer to the font data for the next character from the
        // string.  If there is not a glyph for the next character, replace it
        // with a ".".
        //
        if ((*string >= first) && (*string <= last)) {
            //
            // Add the width of this character as drawn with the given font.
            //
            width += data[offset[*string - first] + 1];
        } else {
            //
            // This character does not exist in the font so replace it with
            // a '.' instead.  This matches the approach taken in GrStringDraw
            // and ensures that the width returned here represents the
            // rendered dimension of the string.
            //
            width += data[offset[absent - first] + 1];
        }
    }

    //
    // Return the width of the string.
    //
    return (width);
}


/******************************************************************************/
void eui_str_draw(eui_context_t *context,
                  const char *string,
                  int length,
                  eui_coord_t x,
                  eui_coord_t y,
                  int opaque)
{
    eui_coord_t idx, x0, y0;
    eui_color_t fg, bg;
    int16_t count, off, on, bit;
    const uint8_t *data;
    const uint8_t *glyphs;
    const uint16_t *offset;
    uint8_t first, last, absent;
    
    int32_t  ySave = y;
    y = 0;
    
    /* we may change these, save them */
    fg = context->fg;
    bg = context->bg;

    //
    // Extract various parameters from the font depending upon whether it's
    // in the tFont or Graphics_FontEx format.
    //
    if (context->font->format & EUI_FONT_EX_MARKER)
    {
        eui_font_ex_t *font = (eui_font_ex_t *)context->font;

        glyphs = font->data;
        offset = font->offset;
        first = font->first;
        last = font->last;

        //
        // Does the default absent character replacement exist in the font?
        //
        if ((EUI_ABSENT_CHAR_REPLACEMENT >= first) &&
            (EUI_ABSENT_CHAR_REPLACEMENT <= last)) {
            //
            // Yes - use the standard character when an absent character is
            // found.
            //
            absent = EUI_ABSENT_CHAR_REPLACEMENT;
        } else {
            //
            // The default absent character is not present in the font so use
            // the first character instead.
            //
            absent = font->first;
        }
    }
    else
    {
        glyphs = context->font->data;
        offset = context->font->offset;
        first = 32;
        last = 126;
        absent = EUI_ABSENT_CHAR_REPLACEMENT;
    }

    //
    // Loop through the characters in the string.
    //
    while (*string && length--) {
        //
        // Stop drawing the string if the right edge of the clipping region has
        // been exceeded.
        //
        if (x > context->clip.x2) {
            break;
        }

        //
        // Get a pointer to the font data for the next character from the
        // string.  If there is not a glyph for the next character, replace it
        // with the "absent" character (usually '.').
        //
        if ((*string >= first) && (*string <= last)) {
            data = (glyphs + offset[*string - first]);
        } else {
            data = (glyphs + offset[absent - first]);
        }
        string++;

        //
        // See if the entire character is to the left of the clipping region.
        //
        if ((x + data[1]) < context->clip.x1) {
            //
            // Increment the X coordinate by the width of the character.
            //
            x += data[1];

            //
            // Go to the next character in the string.
            //
            continue;
        }

        //
        // Loop through the bytes in the encoded data for this glyph.
        //
        for (idx = 2, x0 = 0, bit = 0, y0 = 0; idx < data[0]; ) {
            //
            // See if the bottom of the clipping region has been exceeded.
            //
            if((y + y0) > context->clip.y2) {
                //
                // Stop drawing this character.
                //
                break;
            }

            //
            // See if the font is uncompressed.
            //
            if ((context->font->format & ~EUI_FONT_EX_MARKER) == EUI_FONT_FMT_UNCOMPRESSED) {
                //
                // Count the number of off pixels from this position in the
                // glyph image.
                //
                for (off = 0; idx < data[0]; ) {
                    //
                    // Get the number of zero pixels at this position.
                    //
                    count = eui_get_number_of_leading_zeros(data[idx] << bit);

                    //
                    // If there were more than 8, then it is a "false" result
                    // since it counted beyond the end of the current byte.
                    // Therefore, simply limit it to the number of pixels
                    // remaining in this byte.
                    //
                    if (count > 8) {
                        count = 8 - bit;
                    }

                    //
                    // Increment the number of off pixels.
                    //
                    off += count;

                    //
                    // Increment the bit position within the byte.
                    //
                    bit += count;

                    //
                    // See if the end of the byte has been reached.
                    //
                    if (bit == 8) {
                        //
                        // Advance to the next byte and continue counting off
                        // pixels.
                        //
                        bit = 0;
                        idx++;
                    } else {
                        //
                        // Since the end of the byte was not reached, there
                        // must be an on pixel.  Therefore, stop counting off
                        // pixels.
                        //
                        break;
                    }
                }

                //
                // Count the number of on pixels from this position in the
                // glyph image.
                //
                for (on = 0; idx < data[0]; ) {
                    //
                    // Get the number of one pixels at this location (by
                    // inverting the data and counting the number of zeros).
                    //
                    count = eui_get_number_of_leading_zeros(~(data[idx] << bit));
                    

                    //
                    // If there were more than 8, then it is a "false" result
                    // since it counted beyond the end of the current byte.
                    // Therefore, simply limit it to the number of pixels
                    // remaining in this byte.
                    //
                    if (count > 8) {
                        count = 8 - bit;
                    }

                    //
                    // Increment the number of on pixels.
                    //
                    on += count;

                    //
                    // Increment the bit position within the byte.
                    //
                    bit += count;

                    //
                    // See if the end of the byte has been reached.
                    //
                    if (bit == 8) {
                        //
                        // Advance to the next byte and continue counting on
                        // pixels.
                        //
                        bit = 0;
                        idx++;
                    } else {
                        //
                        // Since the end of the byte was not reached, there
                        // must be an off pixel.  Therefore, stop counting on
                        // pixels.
                        //
                        break;
                    }
                }
            }
            //
            // Otherwise, the font is compressed with a pixel RLE scheme.
            //
            else {
                //
                // See if this is a byte that encodes some on and off pixels.
                //
                if (data[idx]) {
                    //
                    // Extract the number of off pixels.
                    //
                    off = (data[idx] >> 4) & 15;

                    //
                    // Extract the number of on pixels.
                    //
                    on = data[idx] & 15;

                    //
                    // Skip past this encoded byte.
                    //
                    idx++;
                }
                //
                // Otherwise, see if this is a repeated on pixel byte.
                //
                else if (data[idx + 1] & 0x80) {
                    //
                    // There are no off pixels in this encoding.
                    //
                    off = 0;

                    //
                    // Extract the number of on pixels.
                    //
                    on = (data[idx + 1] & 0x7f) * 8;

                    //
                    // Skip past these two encoded bytes.
                    //
                    idx += 2;
                }
                //
                // Otherwise, this is a repeated off pixel byte.
                //
                else {
                    //
                    // Extract the number of off pixels.
                    //
                    off = data[idx + 1] * 8;

                    //
                    // There are no on pixels in this encoding.
                    //
                    on = 0;

                    //
                    // Skip past these two encoded bytes.
                    //
                    idx += 2;
                }
            }

            //
            // Loop while there are any off pixels.
            //
            while (off) {
                //
                // See if the bottom of the clipping region has been exceeded.
                //
                if ((y + y0) > context->clip.y2) {
                    //
                    // Ignore the remainder of the on pixels.
                    //
                    break;
                }

                //
                // See if there is more than one on pixel that will fit onto
                // the current row.
                //
                if ((off > 1) && ((x0 + 1) < data[1])) {
                    //
                    // Determine the number of on pixels that will fit on this
                    // row.
                    //
                    count = (((x0 + off) > data[1]) ? data[1] - x0 : off);

                    //
                    // If this row is within the clipping region, draw a
                    // horizontal line that corresponds to the sequence of on
                    // pixels.
                    //
                    if(((y + y0) >= context->clip.y1) && opaque) {
                        eui_draw_line_h(context, x + x0, x + x0 + count - 1, ySave + y + y0, bg);
                    }

                    //
                    // Decrement the count of on pixels by the number on this
                    // row.
                    //
                    off -= count;

                    //
                    // Increment the X offset by the number of on pixels.
                    //
                    x0 += count;
                }
                //
                // Otherwise, there is only a single on pixel that can be
                // drawn.
                //
                else {
                    //
                    // If this pixel is within the clipping region, then draw
                    // it.
                    //
                    if (((x + x0) >= context->clip.x1) &&
                        ((x + x0) <= context->clip.x2) &&
                        ((y + y0) >= context->clip.y1) && opaque) {
                        eui_draw_pixel(context, x + x0, ySave + y + y0, bg);
                    }

                    //
                    // Decrement the count of on pixels.
                    //
                    off--;

                    //
                    // Increment the X offset.
                    //
                    x0++;
                }

                //
                // See if the X offset has reached the right side of the
                // character glyph.
                //
                if (x0 == data[1]) {
                    //
                    // Increment the Y offset.
                    //
                    y0++;

                    //
                    // Reset the X offset to the left side of the character
                    // glyph.
                    //
                    x0 = 0;
                }
            }

            /* Loop while there are any on pixels. */
            while (on) {
                //
                // See if the bottom of the clipping region has been exceeded.
                //
                if ((y + y0) > context->clip.y2) {
                    /* Ignore the remainder of the on pixels. */
                    break;
                }

                //
                // See if there is more than one on pixel that will fit onto
                // the current row.
                //
                if ((on > 1) && ((x0 + 1) < data[1])) {
                    //
                    // Determine the number of on pixels that will fit on this
                    // row.
                    //
                    count = (((x0 + on) > data[1]) ? data[1] - x0 : on);

                    //
                    // If this row is within the clipping region, draw a
                    // horizontal line that corresponds to the sequence of on
                    // pixels.
                    //
                    if ((y + y0) >= context->clip.y1) {
                        eui_draw_line_h(context, x + x0, x + x0 + count - 1, ySave + y + y0, fg);
                    }

                    //
                    // Decrement the count of on pixels by the number on this
                    // row.
                    //
                    on -= count;

                    //
                    // Increment the X offset by the number of on pixels.
                    //
                    x0 += count;
                }
                //
                // Otherwise, there is only a single on pixel that can be
                // drawn.
                //
                else {
                    //
                    // If this pixel is within the clipping region, then draw
                    // it.
                    //
                    if (((x + x0) >= context->clip.x1) &&
                        ((x + x0) <= context->clip.x2) &&
                        ((y + y0) >= context->clip.y1)) {
                        eui_draw_pixel(context, x + x0, ySave + y + y0, fg);
                    }
          
                    //
                    // Decrement the count of on pixels.
                    //
                    on--;

                    //
                    // Increment the X offset.
                    //
                    x0++;
                }

                //
                // See if the X offset has reached the right side of the
                // character glyph.
                //
                if (x0 == data[1]) {
                    //
                    // Increment the Y offset.
                    //
                    y0++;

                    //
                    // Reset the X offset to the left side of the character
                    // glyph.
                    //
                    x0 = 0;
                }
            }
        }

        //
        // Increment the X coordinate by the width of the character.
        //
        x += data[1];
    }
    
    /* we may have changed these */
    context->fg = fg;
    context->bg = bg;
}


/******************************************************************************/
uint8_t eui_str_baseline(const eui_font_t *font)
{
	return font->baseline;
}


/******************************************************************************/
void eui_str_draw_centered(eui_context_t *context,
                           const char *string, int length, eui_coord_t x, eui_coord_t y,
                           int opaque)
{
    eui_str_draw(context, string, length,
                 (x) - (eui_str_width(context->font, string, length) / 2),
                 (y) - (context->font->baseline / 2), opaque);
}


/******************************************************************************/
uint8_t eui_str_char_max_height(const eui_font_t *font)
{
	return font->height;
}


/******************************************************************************/
uint8_t eui_str_char_max_width(const eui_font_t *font)
{
	return font->maxWidth;
}



