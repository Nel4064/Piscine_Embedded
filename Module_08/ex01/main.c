#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "avr/interrupt.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

// DS40002061B-p.171 / Suggested naming from example
#define SS			PORTB2	// SPI SS (Slave Select) Pin
#define	MOSI		PORTB3	// SPI MOSI (Master Out Slave) In Pin
#define	SCK			PORTB5	// SPI Clock Pin
#define DD_SS		DDB2	// Direction for SS Pin
#define	DD_MOSI		DDB3	// Direction for MOSI Pin
#define	DD_SCK		DDB5	// Direction for Clock Pin
#define DDR_SPI		DDRB	// SPI Direction Register = DDRB

#define START_FRAME	0x00000000
#define STOP_FRAME	0xFFFFFFFF

#define BRIGHTNESS	0xE3000000
#define RED     0x000000FF
#define GREEN   0x0000FF00
#define BLUE    0x00FF0000
#define BLANK	0x00000000
#define YELLOW	(RED | GREEN)
#define CYAN	(GREEN | BLUE)
#define MAGENTA	(RED | BLUE)
#define WHITE	(RED | GREEN | BLUE)

// DS40002061B-p.172 / Example 
void	spi_master_init(void)
{
	// // Setting SS (Slave Select) Pin as Output
	DDR_SPI |= (1 << DD_SS); 
	// // DS40002061B-p.174 / Master Mode / If SS is configured as an output, the pin is a general output pin which does not affect the SPI system.
	// // DS40002061B-p.85 / If DDxn is written logic one, Pxn is configured as an output pin
	// // Note: no such pin for the SK9822. We set DD_SS and we are done.

	// Setting SPI MOSI and Clock Pin as Output
	DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK);
	// DS40002061B-p.172 - Example

	// // Setting Data Modes / SPI Transfert Formats (polarity of clock and falling edge for sampling)
	// // CPOL=1, CPHA=1 is the mode correcponding to the schematic of SPC_SK9822_Rev.01-p.5
	SPCR |= (1 << CPOL); // DS40002061B-p.176 Trailing Edge Rising
	SPCR |= (1 << CPHA); // DS40002061B-p.176 Trailing Edge = Sample

	// Enable SPI, Master, set clock rate fck/16
	SPCR |= (1 << MSTR);   // DS40002061B-p.176 Master SPI mode when written to one
	SPSR &= ~(1 << SPI2X); // DS40002061B-p.177 SPI2X/SPR1/SPR0 = 0b010 => Set fosc/16 = 1MHZ
	SPCR &= ~(1 << SPR1);   // DS40002061B-p.177 Idem above
	SPCR |= (1 << SPR0);   // DS40002061B-p.177 Idem above
	SPCR &= ~(1 << DORD);  // DS40002061B-p.176 When the DORD bit is written to zero, the MSB of the data word is transmitted first
	SPCR &= ~(1 << SPIE);  // DS40002061B-p.176 SPIE: SPI Interrupt Not Enabled
	SPCR |= (1 << SPE);    // DS40002061B-p.176 When the SPE bit is written to one, the SPI is enabled.
	// SPC_SK9822_Rev.01-p.2 The maximum frequency of 30MHZ serial data input
}

// DS40002061B-p.172 / Example 
void	spi_master_transmit(uint8_t data)
{
	// Start transmission = SPDR = SPI Data Register
	SPDR = data; // DS40002061B-p.178 Writing to the register initiates data transmission

	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF))); // DS40002061B-p.177 When a serial transfer is complete, the SPIF Flag is set.
}

void	spi_tx_uint32(uint32_t data)
{
	spi_master_transmit((uint8_t)((data & 0xFF000000) >> 24)); // Byte 4 (MSB)
	spi_master_transmit((uint8_t)((data & 0x00FF0000) >> 16));
	spi_master_transmit((uint8_t)((data & 0x0000FF00) >> 8));
	spi_master_transmit((uint8_t)((data & 0x000000FF) >> 0));  // Byte 0 (LSB)
}

void led_blinking_sequence(void)
{
	uint32_t patterns[] = 
	{
		(BRIGHTNESS | RED),
		(BRIGHTNESS | GREEN),
		(BRIGHTNESS | BLUE),
		(BRIGHTNESS | YELLOW),
		(BRIGHTNESS | CYAN),
		(BRIGHTNESS | MAGENTA),
		(BRIGHTNESS | WHITE)
	};

	for (uint8_t i = 0; i < 7; i++)
	{
		spi_tx_uint32(START_FRAME);
		spi_tx_uint32(patterns[i]); // D6
		spi_tx_uint32(BRIGHTNESS | BLANK); // D7
		spi_tx_uint32(BRIGHTNESS | BLANK); // D8
		spi_tx_uint32(STOP_FRAME);
		_delay_ms(1000);
	}
}

int main(void)
{
	uart_init();
	spi_master_init();
	while (1)
		led_blinking_sequence();
}


// SK9822 Data Sheet : https://www.pololu.com/file/0J1234/sk9822_datasheet.pdf