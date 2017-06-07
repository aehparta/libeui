/* --COPYRIGHT--,BSD
 * Copyright (c) 2013, Texas Instruments Incorporated
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
//*****************************************************************************
//
// SSD1289_Driver.c - Display driver for SSD1289 LCD Controller.
//
//*****************************************************************************
//
//! \addtogroup display_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// READ ME
//
// This template driver is intended to be modified for creating new LCD drivers
// It is setup so that only SSD1289_DriverPixelDraw() and DPYCOLORTRANSLATE()
// and some LCD size configuration settings in the header file SSD1289_Driver.h
// are REQUIRED to be written. These functions are marked with the string
// "TemplateDisplayFix" in the comments so that a search through SSD1289_Driver.c and
// SSD1289_Driver.h can quickly identify the necessary areas of change.
//
// SSD1289_DriverPixelDraw() is the base function to write to the LCD
// display. Functions like WriteData(), WriteCommand(), and SetAddress()
// are suggested to be used to help implement the SSD1289_DriverPixelDraw()
// function, but are not required. SetAddress() should be used by other pixel
// level functions to help optimize them.
//
// This is not an optimized driver however and will significantly impact
// performance. It is highly recommended to first get the prototypes working
// with the single pixel writes, and then go back and optimize the driver.
// Please see application note www.ti.com/lit/pdf/slaa548 for more information
// on how to fully optimize LCD driver files. In int16_t, driver optimizations
// should take advantage of the auto-incrementing of the LCD controller.
// This should be utilized so that a loop of WriteData() can be used instead
// of a loop of SSD1289_DriverPixelDraw(). The pixel draw loop contains both a
// SetAddress() + WriteData() compared to WriteData() alone. This is a big time
// saver especially for the line draws and SSD1289_DriverPixelDrawMultiple.
// More optimization can be done by reducing function calls by writing macros,
// eliminating unnecessary instructions, and of course taking advantage of other
// features offered by the LCD controller. With so many pixels on an LCD screen
// each instruction can have a large impact on total drawing time.
//
//*****************************************************************************


//*****************************************************************************
//
// Include Files
//
//*****************************************************************************
#include <msp430.h>
#include "grlib.h"
#include "SSD1289_Driver.h"

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
/* Global buffer for the display. This is especially useful on 1BPP, 2BPP, and 4BPP
  displays. This allows you to update pixels while reading the neighboring pixels
  from the buffer instead of a read from the LCD controller. A buffer is not required
  as a read followed by a write can be used instead.*/
//uint8_t Template_Memory[(LCD_X_SIZE * LCD_Y_SIZE * BPP + 7) / 8];



//*****************************************************************************
//
// Suggested functions to help facilitate writing the required functions below
//
//*****************************************************************************

// Reset LCD controller
static void LCDReset()
{
	LCD_RESET_PORT |= LCD_RESET_PIN;
	__delay_cycles(100000);
	LCD_RESET_PORT &= ~LCD_RESET_PIN;
	__delay_cycles(100000);
	LCD_RESET_PORT |= LCD_RESET_PIN;
	__delay_cycles(100000);
}


// Writes data to the LCD controller
static inline void WriteData(uint16_t usData)
{
#if DATA_PORT_WIDTH == 8
	LCD_DATA_PORT = (usData >> 8); /* hight byte */
	LCD_CMD_PORT &= ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR);
	LCD_CMD_PORT |= LCD_CMD_PORT_WR;
	LCD_DATA_PORT = (usData & 0xff); /* low byte */
	LCD_CMD_PORT &= ~LCD_CMD_PORT_WR;
	LCD_CMD_PORT |= LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#endif
#if DATA_PORT_WIDTH == 16
	LCD_DATA_PORT = usData;
	LCD_CMD_PORT &= ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR);
	LCD_CMD_PORT |= LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#endif
}


// Writes a command to the LCD controller
static inline void WriteCommand(uint8_t ucCommand)
{
#if DATA_PORT_WIDTH == 8
	LCD_DATA_PORT = 0x00; /* hight byte */
	LCD_CMD_PORT &= ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RS);
	LCD_CMD_PORT |= LCD_CMD_PORT_WR;
	LCD_DATA_PORT = ucCommand; /* low byte */
	LCD_CMD_PORT &= ~LCD_CMD_PORT_WR;
	LCD_CMD_PORT |= LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#endif
#if DATA_PORT_WIDTH == 16
	LCD_DATA_PORT = ucCommand;
	LCD_CMD_PORT &= ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RS);
	LCD_CMD_PORT |= LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#endif
}

// Writes a command and data to the LCD controller
static inline void WriteCommandAndData(uint8_t ucCommand, uint16_t usData)
{
	WriteCommand(ucCommand);
	WriteData(usData);
}


// Sets the pixel address of the LCD driver
static void SetAddress(int16_t lX, int16_t lY)
{
	/* This function typically writes commands (using WriteCommand()) to the
	LCD to tell it where to move the cursor for the next pixel to be drawn. */
}



/******************************************************************************/
/** Sets the window where next data is to be drawn. */
static void SetWindow(const tRectangle *pRect)
{
	WriteCommandAndData(0x44, (pRect->sYMax << 8) + pRect->sYMin);
	WriteCommandAndData(0x45, pRect->sXMin);
	WriteCommandAndData(0x46, pRect->sXMax);
	WriteCommandAndData(0x4e, pRect->sYMin);
	WriteCommandAndData(0x4f, pRect->sXMin);
	WriteCommand(0x22);
}


// Initializes the pins required for the GPIO-based LCD interface.
// This function configures the GPIO pins used to control the LCD display
// when the basic GPIO interface is in use. On exit, the LCD controller
// has been reset and is ready to receive command and data writes.
static void InitGPIOLCDInterface(void)
{
	/* lcd reset pin */
	LCD_RESET_PORT_DIR |= LCD_RESET_PIN;
	LCD_RESET_PORT |= LCD_RESET_PIN;

	/* lcd backlight pin */
	LCD_BACKLIGHT_PORT_DIR |= LCD_BACKLIGHT_PIN;
	LCD_BACKLIGHT_PORT |= LCD_BACKLIGHT_PIN; /* as default, backlight off(!) */

	/* setup lcd control pins as outputs */
	LCD_CMD_PORT_DIR |= LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
	LCD_CMD_PORT |= LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;

	/* setup lcd data pins as outputs */
#if DATA_PORT_WIDTH == 8
	LCD_DATA_PORT_DIR = 0xff;
	LCD_DATA_PORT = 0x00;
#endif
#if DATA_PORT_WIDTH == 16
	LCD_DATA_PORT_DIR = 0xffff;
	LCD_DATA_PORT = 0x0000;
#endif

	/* reset lcd */
	LCDReset();
}


// Initialize DisplayBuffer.
// This function initializes the display buffer and discards any cached data.
static void InitLCDDisplayBuffer(void *pvDisplayData, uint16_t ulValue)
{
	/*  uint16_t i=0,j=0;
	    for(i =0; i< LCD_Y_SIZE; i++)
	    for(j =0; j< (LCD_X_SIZE * BPP + 7) / 8; j++)
	        Template_Memory[i * LCD_Y_SIZE + j] = ulValue;*/
}


// Set backlight on/off
void SSD1289_DriverBacklightSet(int on_off)
{
	if (on_off)   /* low is on */
	{
		LCD_BACKLIGHT_PORT &= ~LCD_BACKLIGHT_PIN;
	}
	else     /* high is off */
	{
		LCD_BACKLIGHT_PORT |= LCD_BACKLIGHT_PIN;
	}
}


// Initializes the display driver.
void SSD1289_DriverInit()
{
	InitGPIOLCDInterface();
//    InitLCDDisplayBuffer();

	/* configure lcd registers */

	WriteCommandAndData(0x00, 0x0001);
	WriteCommandAndData(0x03, 0xaaac);
	WriteCommandAndData(0x0C, 0x0004);
	__delay_cycles(50000);
	WriteCommandAndData(0x0D, 0x000a);
	WriteCommandAndData(0x0E, 0x2c00);
	WriteCommandAndData(0x1E, 0x00B8);
	__delay_cycles(50000);
#ifdef LANDSCAPE
	WriteCommandAndData(0x01, 0x6b3f);
	WriteCommandAndData(0x11, 0x6830);
#endif
#ifdef LANDSCAPE_FLIP
	WriteCommandAndData(0x01, 0x293f);
	WriteCommandAndData(0x11, 0x6830);
#endif
#ifdef PORTRAIT
	WriteCommandAndData(0x01, 0x2b3f);
	WriteCommandAndData(0x11, 0x6830);
#endif
#ifdef PORTRAIT_FLIP
	WriteCommandAndData(0x01, 0x693f);
	WriteCommandAndData(0x11, 0x6830);
#endif
	WriteCommandAndData(0x02, 0x0600);
	WriteCommandAndData(0x10, 0x0000);
	__delay_cycles(50000);
	WriteCommandAndData(0x05, 0x0000);
	WriteCommandAndData(0x06, 0x0000);
	WriteCommandAndData(0x16, 0xEF1C);
	WriteCommandAndData(0x17, 0x0103);
	WriteCommandAndData(0x07, 0x0233);
	WriteCommandAndData(0x0B, 0x5312);
	WriteCommandAndData(0x0F, 0x0000);
	__delay_cycles(50000);
	WriteCommandAndData(0x41, 0x0000);
	WriteCommandAndData(0x42, 0x0000);
	WriteCommandAndData(0x48, 0x0000);
	WriteCommandAndData(0x49, 0x013f);
	WriteCommandAndData(0x4A, 0x0000);
	WriteCommandAndData(0x4B, 0x013f);
	WriteCommandAndData(0x44, 0xEF00);
	WriteCommandAndData(0x45, 0x0000);
	WriteCommandAndData(0x46, 0x013F);
	__delay_cycles(50000);
	WriteCommandAndData(0x30, 0x0707);
	WriteCommandAndData(0x31, 0x0704);
	WriteCommandAndData(0x32, 0x0204);
	WriteCommandAndData(0x33, 0x0201);
	WriteCommandAndData(0x34, 0x0203);
	WriteCommandAndData(0x35, 0x0204);
	WriteCommandAndData(0x36, 0x0204);
	WriteCommandAndData(0x37, 0x0502);
	WriteCommandAndData(0x3A, 0x0302);
	WriteCommandAndData(0x3B, 0x0500);
	__delay_cycles(50000);
	WriteCommandAndData(0x23, 0x0000);
	WriteCommandAndData(0x24, 0x0000);
	WriteCommandAndData(0x25, 0x8000);
	WriteCommandAndData(0x4f, 0x0000);
	WriteCommandAndData(0x4e, 0x0000);
	__delay_cycles(50000);
}




//*****************************************************************************
//
// All the following functions (below) for the LCD driver are required by grlib
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
// TemplateDisplayFix
static void SSD1289_DriverPixelDraw(void *pvDisplayData, int16_t lX, int16_t lY, uint16_t ulValue)
{
	tRectangle rect;

	rect.sXMin = MAPPED_X(lX, lY);
	rect.sXMax = MAPPED_X(lX, lY);
	rect.sYMin = MAPPED_Y(lX, lY);
	rect.sYMax = MAPPED_Y(lX, lY);

	SetWindow(&rect);
	WriteData(ulValue);

	/* This function already has checked that the pixel is within the extents of
	the LCD screen and the color ulValue has already been translated to the LCD.
	This function typically looks like:

	// Interpret pixel data (if needed)

	// Update buffer (if applicable)
	// Template_Memory[lY * LCD_Y_SIZE + (lX * BPP / 8)] = , |= , &= ...
	// Template memory must be modified at the bit level for 1/2/4BPP displays

	// SetAddress(MAPPED_X(lX, lY), MAPPED_Y(lX, lY));
	// WriteData(ulValue);
	*/
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
SSD1289_DriverPixelDrawMultiple(void *pvDisplayData, int16_t lX, int16_t lY, int16_t lX0, int16_t lCount, int16_t lBPP, const uint8_t *pucData, const uint32_t *pucPalette)
{
	uint16_t ulByte;
	tRectangle rect;

	rect.sXMin = MAPPED_X(lX, lY);
	rect.sXMax = MAPPED_X(lX, lY)  + lCount - 1;
	rect.sYMin = MAPPED_Y(lX, lY);
	rect.sYMax = MAPPED_Y(lX, lY);

	SetWindow(&rect);

	//
	// Determine how to interpret the pixel data based on the number of bits
	// per pixel.
	//
	switch (lBPP)
	{
	// The pixel data is in 1 bit per pixel format
	case 1:
	{
		// Loop while there are more pixels to draw
		while (lCount > 0)
		{
			// Get the next byte of image data
			ulByte = *pucData++;

			// Loop through the pixels in this byte of image data
			for (; (lX0 < 8) && lCount; lX0++, lCount--)
			{
				WriteData(((uint16_t *)pucPalette)[(ulByte >> (7 - lX0)) & 1]);
				lX++;
				// Draw this pixel in the appropriate color
				//SSD1289_DriverPixelDraw(pvDisplayData, lX++, lY,
				//                      ((uint16_t *)pucPalette)[(ulByte >> (7 - lX0)) & 1]);
			}

			// Start at the beginning of the next byte of image data
			lX0 = 0;
		}
		// The image data has been drawn

		break;
	}

	// The pixel data is in 2 bit per pixel format
	case 2:
	{
		// Loop while there are more pixels to draw
		while (lCount > 0)
		{
			// Get the next byte of image data
			ulByte = *pucData++;

			// Loop through the pixels in this byte of image data
			for (; (lX0 < 4) && lCount; lX0++, lCount--)
			{
				WriteData(((uint16_t *)pucPalette)[(ulByte >> (6 - (lX0 << 1))) & 3]);
				lX++;
				// Draw this pixel in the appropriate color
				//SSD1289_DriverPixelDraw(pvDisplayData, lX++, lY,
				//                      ((uint16_t *)pucPalette)[(ulByte >> (6 - (lX0 << 1))) & 3]);
			}

			// Start at the beginning of the next byte of image data
			lX0 = 0;
		}
		// The image data has been drawn

		break;
	}
	// The pixel data is in 4 bit per pixel format
	case 4:
	{
		// Loop while there are more pixels to draw.  "Duff's device" is
		// used to jump into the middle of the loop if the first nibble of
		// the pixel data should not be used.  Duff's device makes use of
		// the fact that a case statement is legal anywhere within a
		// sub-block of a switch statement.  See
		// http://en.wikipedia.org/wiki/Duff's_device for detailed
		// information about Duff's device.
		switch (lX0 & 1)
		{
		case 0:

			while (lCount)
			{
				// Get the upper nibble of the next byte of pixel data
				// and extract the corresponding entry from the palette
				ulByte = (*pucData >> 4);
				ulByte = (*(uint16_t *)(pucPalette + ulByte));
				// Write to LCD screen
				WriteData(ulByte);
				lX++;
				//SSD1289_DriverPixelDraw(pvDisplayData, lX++, lY, ulByte);

				// Decrement the count of pixels to draw
				lCount--;

				// See if there is another pixel to draw
				if (lCount)
				{
				case 1:
					// Get the lower nibble of the next byte of pixel
					// data and extract the corresponding entry from
					// the palette
					ulByte = (*pucData++ & 15);
					ulByte = (*(uint16_t *)(pucPalette + ulByte));
					// Write to LCD screen
					WriteData(ulByte);
					lX++;
					//SSD1289_DriverPixelDraw(pvDisplayData, lX++, lY, ulByte);

					// Decrement the count of pixels to draw
					lCount--;
				}
			}
		}
		// The image data has been drawn.

		break;
	}

	// The pixel data is in 8 bit per pixel format
	case 8:
	{
		// Loop while there are more pixels to draw
		while (lCount--)
		{
			// Get the next byte of pixel data and extract the
			// corresponding entry from the palette
			ulByte = *pucData++;
			ulByte = (*(uint16_t *)(pucPalette + ulByte));
			// Write to LCD screen
			WriteData(ulByte);
			lX++;
			//SSD1289_DriverPixelDraw(pvDisplayData, lX++, lY, ulByte);
		}
		// The image data has been drawn
		break;
	}
	}
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void SSD1289_DriverLineDrawH(void *pvDisplayData, int16_t lX1, int16_t lX2, int16_t lY, uint16_t ulValue)
{
	tRectangle rect;

	rect.sXMin = MAPPED_X(lX1, lY);
	rect.sXMax = MAPPED_X(lX2, lY);
	rect.sYMin = MAPPED_Y(lX1, lY);
	rect.sYMax = MAPPED_Y(lX2, lY);

	SetWindow(&rect);

#if DATA_PORT_WIDTH == 16
	LCD_DATA_PORT = ulValue;
#endif

	for ( ; lX1 <= lX2; lX1++)
	{
#if DATA_PORT_WIDTH == 16
		LCD_CMD_PORT = LCD_CMD_PORT & ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR);
		LCD_CMD_PORT = LCD_CMD_PORT | LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#else
		WriteData(ulValue);
#endif
	}
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void SSD1289_DriverLineDrawV(void *pvDisplayData, int16_t lX, int16_t lY1, int16_t lY2, uint16_t ulValue)
{
	tRectangle rect;

	rect.sXMin = MAPPED_X(lX, lY1);
	rect.sXMax = MAPPED_X(lX, lY2);
	rect.sYMin = MAPPED_Y(lX, lY1);
	rect.sYMax = MAPPED_Y(lX, lY2);

	SetWindow(&rect);

#if DATA_PORT_WIDTH == 16
	LCD_DATA_PORT = ulValue;
#endif

	for ( ; lY1 <= lY2; lY1++)
	{
#if DATA_PORT_WIDTH == 16
		LCD_CMD_PORT = LCD_CMD_PORT & ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR);
		LCD_CMD_PORT = LCD_CMD_PORT | LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#else
		WriteData(ulValue);
#endif
	}
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void SSD1289_DriverRectFill(void *pvDisplayData, const tRectangle *pRect, uint16_t ulValue)
{
	int x, y;
	tRectangle rect;

	rect.sXMin = MAPPED_X(pRect->sXMin, pRect->sYMin);
	rect.sXMax = MAPPED_X(pRect->sXMax, pRect->sYMax);
	rect.sYMin = MAPPED_Y(pRect->sXMin, pRect->sYMin);
	rect.sYMax = MAPPED_Y(pRect->sXMax, pRect->sYMax);

	SetWindow(&rect);

#if DATA_PORT_WIDTH == 16
	LCD_DATA_PORT = ulValue;
#endif

	for (y = 0; y <= (pRect->sYMax - pRect->sYMin); y++)
	{
		for (x = 0; x <= (pRect->sXMax - pRect->sXMin); x++)
		{
#if DATA_PORT_WIDTH == 16
			LCD_CMD_PORT = LCD_CMD_PORT & ~(LCD_CMD_PORT_CS | LCD_CMD_PORT_WR);
			LCD_CMD_PORT = LCD_CMD_PORT | LCD_CMD_PORT_CS | LCD_CMD_PORT_RS | LCD_CMD_PORT_WR | LCD_CMD_PORT_RD;
#else
			WriteData(ulValue);
#endif
		}
	}
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static uint32_t SSD1289_DriverColorTranslate(void *pvDisplayData, uint32_t  ulValue)
{
	//
	// Translate from a 24-bit RGB color to a color accepted by the LCD.
	//
	return (DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.
//!
//! \return None.
//
//*****************************************************************************
static void SSD1289_DriverFlush(void *pvDisplayData)
{
	// Flush Buffer here. This function is not needed if a buffer is not used,
	// or if the buffer is always updated with the screen writes.
	/*  int16_t i=0,j=0;
	    for(i =0; i< LCD_Y_SIZE; i++)
	    for(j =0; j< (LCD_X_SIZE * BPP + 7) / 8; j++)
	        SSD1289_DriverPixelDraw(pvDisplayData, j, i, Template_Memory[i * LCD_Y_SIZE + j]);*/
}

//*****************************************************************************
//
//! Send command to clear screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This function does a clear screen and the Display Buffer contents
//! are initialized to the current background color.
//!
//! \return None.
//
//*****************************************************************************
static void SSD1289_DriverClearScreen(void *pvDisplayData, uint16_t ulValue)
{
	tRectangle rect;

	rect.sXMin = 0;
	rect.sXMax = MAPPED_X(LCD_X_SIZE - 1, LCD_Y_SIZE - 1);
	rect.sYMin = 0;
	rect.sYMax = MAPPED_Y(LCD_X_SIZE - 1, LCD_Y_SIZE - 1);

	SSD1289_DriverRectFill(pvDisplayData, &rect, ulValue);
}

//*****************************************************************************
//
//! The display structure that describes the driver for the blank template.
//
//*****************************************************************************
const Graphics_Display g_sSSD1289_Driver =
{
	sizeof(Graphics_Display),
	0,
#if defined(PORTRAIT) || defined(PORTRAIT_FLIP)
	LCD_Y_SIZE,
	LCD_X_SIZE,
#else
	LCD_X_SIZE,
	LCD_Y_SIZE,
#endif
	SSD1289_DriverPixelDraw,
	SSD1289_DriverPixelDrawMultiple,
	SSD1289_DriverLineDrawH,
	SSD1289_DriverLineDrawV,
	SSD1289_DriverRectFill,
	SSD1289_DriverColorTranslate,
	SSD1289_DriverFlush,
	SSD1289_DriverClearScreen
};

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
