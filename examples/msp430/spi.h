 /*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

#ifndef _SPI_H
#define _SPI_H

/******************************************************************************/
#include <msp430.h>
#include <stdint.h>


/******************************************************************************/
/* spi pins */
#define SPI_SIMO        BIT4
#define SPI_SOMI        BIT5
#define SPI_CLK         BIT0
#define TOUCH_CS        BIT5
#define SD_CS           BIT4

/* spi ports */
#define SPI_SEL         P4SEL
#define SPI_DIR         P4DIR
#define SPI_OUT         P4OUT
#define SPI_REN         P4REN
#define TOUCH_CS_SEL    P3SEL
#define TOUCH_CS_OUT    P3OUT
#define TOUCH_CS_DIR    P3DIR
#define SD_CS_SEL       P3SEL
#define SD_CS_OUT       P3OUT
#define SD_CS_DIR       P3DIR


/******************************************************************************/
void spi_init();

/**
 * Enable fast SPI transfers. This function is typically
 * called after the initial SD Card setup is done to maximize
 * transfer speed.
 */
void spi_fastmode(void);

/**
 * Read a frame of bytes via SPI.
 * @param pBuffer Place to store the received bytes
 * @param size Indicator of how many bytes to receive
 */
void spi_read(uint8_t *buffer, uint16_t size);

/***************************************************************************/
/**
 * Write a frame of bytes via SPI.
 * @param   pBuffer Place that holds the bytes to send
 * @param   size Indicator of how many bytes to send
 */
void spi_write(uint8_t *buffer, uint16_t size);


#endif /* _SPI_H - END OF HEADER FILE */
/******************************************************************************/
