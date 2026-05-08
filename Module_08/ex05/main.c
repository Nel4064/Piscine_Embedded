#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "adv.h"
#include "mini_libft.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

// DS40002061B-p.171 / Suggested naming from example
#define DD_SS		DDB2	// Direction for SS Pin
#define	DD_MOSI		DDB3	// Direction for MOSI Pin
#define	DD_SCK		DDB5	// Direction for Clock Pin
#define DDR_SPI		DDRB	// SPI Direction Register = DDRB

#define START_FRAME	0x00000000
#define STOP_FRAME	0xFFFFFFFF

#define BRIGHTNESS	0xE1000000
#define RED		0x000000FF
#define GREEN	0x0000FF00
#define BLUE	0x00FF0000
#define BLANK	0x00000000
#define YELLOW	(RED | GREEN)
#define CYAN	(GREEN | BLUE)
#define MAGENTA	(RED | BLUE)
#define WHITE	(RED | GREEN | BLUE)

// Global variables to track the current RGB color and LED
uint8_t g_current_rgb = 0; // 0: Red, 1: Green, 2: Blue
uint8_t g_current_led = 0; // 0: D6, 1: D7, 2: D8
uint32_t g_rgb_values[3][3] = {0}; // Stores the values for R, G, B values (0-255)

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

void	sw1_and_sw2_init(void)
{
	// Buttons (PD2/SW1, PD4/SW2) = INPUT + ENABLED
	DDRD &= ~((1 << DDD2) | (1 << DDD4));
	PORTD |= (1 << PORTD2) | (1 << PORTD4);
}

void switch_to_next_rgb()
{
	g_current_rgb = (g_current_rgb + 1) % 3;
}

void switch_to_next_led()
{
	g_current_led = (g_current_led + 1) % 3;
}

// DS40002061B-p.74 Interrupt Vectors = ISR name
__attribute__((signal, used))
void	ADC_vect()
{
	static uint32_t color_led_d6 = BRIGHTNESS | BLANK;
	static uint32_t color_led_d7 = BRIGHTNESS | BLANK;
	static uint32_t color_led_d8 = BRIGHTNESS | BLANK;

	g_rgb_values[g_current_rgb][g_current_led] = ADCH;

	TIFR1 |= (1 << OCF1B); // DS40002061B-page 145 Clear OCF1B flag
	// => OCF1B is "not" automatically cleared when the Output Compare Match B Interrupt Vector is "not" executed. Alternatively, OCF1B can be cleared by writing a logic one to its bit location.

	switch (g_current_led)
	{
		case 0: // D6
			color_led_d6 = BRIGHTNESS | (g_rgb_values[0][0] << 0) | (g_rgb_values[1][0] << 8) | (g_rgb_values[2][0] << 16);
			break ;
		case 1: // D7
			color_led_d7 = BRIGHTNESS | (g_rgb_values[0][1] << 0) | (g_rgb_values[1][1] << 8) | (g_rgb_values[2][1] << 16);
			break ;
		case 2: // D8
			color_led_d8 = BRIGHTNESS | (g_rgb_values[0][2] << 0) | (g_rgb_values[1][2] << 8) | (g_rgb_values[2][2] << 16);
			break ;
	}

	spi_tx_uint32(START_FRAME);
	spi_tx_uint32(color_led_d6); // D6
	spi_tx_uint32(color_led_d7); // D7
	spi_tx_uint32(color_led_d8); // D8
	spi_tx_uint32(STOP_FRAME);
}

int main(void)
{
	// 0 = not pressed, 1 = pressed
	uint8_t	was_button_SW1_pressed = 0;
	uint8_t	was_button_SW2_pressed = 0;

	uart_init();
	spi_master_init();
	init_timer1(); // for ADC auto-trigger every 20ms on MUX = ADC0 => RV1
	init_adc();
	sw1_and_sw2_init();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Status Register / = sei() from <avr/interrupt.h>

	while (1)
	{
		uint8_t	is_button_SW1_pressed = !(PIND & (1 << PD2));
		uint8_t	is_button_SW2_pressed = !(PIND & (1 << PD4));

		if (!was_button_SW1_pressed && is_button_SW1_pressed)
		{
			_delay_ms(20);
			is_button_SW1_pressed = !(PIND & (1 << PIND2));
			if (is_button_SW1_pressed)
				switch_to_next_rgb();
		}

		if (!was_button_SW2_pressed && is_button_SW2_pressed)
		{
			_delay_ms(20);
			is_button_SW2_pressed = !(PIND & (1 << PIND4));
			if (is_button_SW2_pressed)
				switch_to_next_led();
		}
		was_button_SW1_pressed = is_button_SW1_pressed;
		was_button_SW2_pressed = is_button_SW2_pressed;
	}
}
