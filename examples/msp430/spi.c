/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "spi.h"


/******************************************************************************/
void spi_init()
{
	uint16_t spi_divider = 0;

	/* calculate spi clock divider from SMCLK,
	 * initial clock must be less than 400kHz */
	spi_divider = (uint16_t)(SMCLK_FREQ / 400000);

	/* initialize spi stuff */
	SPI_SEL |= SPI_CLK + SPI_SOMI + SPI_SIMO;
	SPI_DIR |= SPI_CLK + SPI_SIMO;
	SPI_REN |= SPI_SOMI; /* pull-ups for sd cards on SOMI */
	SPI_OUT |= SPI_SOMI; /* certain sd card brands need pull-ups */

	/* touch screen cs */
	TOUCH_CS_SEL &= ~TOUCH_CS;
	TOUCH_CS_OUT |= TOUCH_CS;
	TOUCH_CS_DIR |= TOUCH_CS;
	/* sd card cs */
	SD_CS_SEL &= ~SD_CS;
	SD_CS_OUT |= SD_CS;
	SD_CS_DIR |= SD_CS;

	/* initialize USCI_A1 for spi master operation */
	UCA1CTL1 |= UCSWRST; /* put state machine in reset */
	UCA1CTL0 = UCCKPL + UCMSB + UCMST + UCMODE_0 + UCSYNC; /* 3-pin, 8-bit spi master */
	/* clock polarity select - the inactive state is high */
	/* msb first */
	UCA1CTL1 = UCSWRST + UCSSEL_2; /* use SMCLK, keep RESET */
	UCA1BR0 = spi_divider & 0xff;
	UCA1BR1 = spi_divider >> 8;
	UCA1CTL1 &= ~UCSWRST;          /* release USCI state machine */
	UCA1IFG &= ~UCRXIFG;
}


/***************************************************************************/
void spi_fastmode(void)
{
	UCA1CTL1 |= UCSWRST;  /* put state machine in reset */
	UCA1BR0 = 2;          /* f_UCxCLK = 24MHz/2 = 12MHz */
	UCA1BR1 = 0;
	UCA1CTL1 &= ~UCSWRST; /* release USCI state machine */
}


/***************************************************************************/
/**
 * Read a frame of bytes via SPI.
 * @param p Place to store the received bytes
 * @param size Indicator of how many bytes to receive
 */
void spi_read(uint8_t *p, uint16_t size)
{
	uint16_t gie = __get_SR_register() & GIE; /* store current GIE state */
	__disable_interrupt(); /* make this operation atomic */

	UCA1IFG &= ~UCRXIFG;                                   // Ensure RXIFG is clear

	/* clock the actual data transfer and receive the bytes */
	while (size--)
	{
		while (!(UCA1IFG & UCTXIFG));                     // Wait while not ready for TX
		UCA1TXBUF = 0xff;                                 // Write dummy byte
		while (!(UCA1IFG & UCRXIFG));                     // Wait for RX buffer (full)
		*p++ = UCA1RXBUF;
	}

	__bis_SR_register(gie); /* restore original GIE state */
}

/***************************************************************************/
/**
 * Write a frame of bytes via SPI.
 * @param   p Place that holds the bytes to send
 * @param   size Indicator of how many bytes to send
 */
void spi_write(uint8_t *p, uint16_t size)
{
	uint16_t gie = __get_SR_register() & GIE; /* store current GIE state */
	__disable_interrupt(); /* make this operation atomic */

	/* Clock the actual data transfer and send the bytes. Note that we
	 * intentionally not read out the receive buffer during frame transmission
	 * in order to optimize transfer speed, however we need to take care of the
	 * resulting overrun condition.
	 */
	while (size--)
	{
		while (!(UCA1IFG & UCTXIFG));                     // Wait while not ready for TX
		UCA1TXBUF = *p++;                            // Write byte
	}
	while (UCA1STAT & UCBUSY);                            // Wait for all TX/RX to finish

	UCA1RXBUF;                                             // Dummy read to empty RX buffer
	// and clear any overrun conditions

	__bis_SR_register(gie); /* restore original GIE state */
}
