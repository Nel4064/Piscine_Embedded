#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "adv.h"
#include "mini_libft.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define MAX_STR_LENGTH	20 // Min. MAX_RGB_LENGHT + "Dx" (i.e. =2) + '\0' (i.e. = 1)  + whatever we want
#define MAX_RGB_LENGTH	6
#define TRUE  		((uint8_t)1)
#define FALSE 		((uint8_t)0)

#define STANDARD	0
#define RAINBOW		1
#define INVALID		2

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

#define BRIGHTNESS	0xE1000000
#define RED		0x000000FF
#define GREEN	0x0000FF00
#define BLUE	0x00FF0000
#define BLANK	0x00000000
#define YELLOW	(RED | GREEN)
#define CYAN	(GREEN | BLUE)
#define MAGENTA	(RED | BLUE)
#define WHITE	(RED | GREEN | BLUE)

// ANSI color codes
#define ANSI_RESET			"\033[0m"
#define ANSI_RED			"\033[31m"
#define ANSI_GREEN			"\033[32m"
#define ANSI_YELLOW			"\033[33m"
#define ANSI_BLUE			"\033[34m"
#define ANSI_PURPLE			"\033[35m"
#define ANSI_CYAN			"\033[36m"

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

uint8_t hex_char_to_val(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return (0);
}

uint8_t get_and_test_str(char* str, uint32_t* r, uint32_t* g, uint32_t* b, uint8_t* led_id)
{
	char c;
	uint8_t i = 0;
	uint8_t is_color_hex = FALSE;
	uint8_t is_led_id_valid = FALSE;

	while (i < MAX_STR_LENGTH - 1)
	{
		c = uart_rx();
		if (c == 127) // if = DEL (backspace)
		{
			if (i > 0)
			{
				uart_printstr("\b \b");
				i--;
			}
			continue ;
		}
		if (c == '\r')
			break ;
		str[i] = c;
		uart_tx(c);
		i++;
	}
	str[i] = '\0';
	uart_printstr("\r\n");

	if (ft_strcmp(str, "#FULLRAINBOW") == 0 || ft_strcmp(str, "R") == 0)
		return(RAINBOW);

	if (str[0] == '#' && i <= MAX_STR_LENGTH - 1) // '#' + 6 hex digits
	{
		is_color_hex = TRUE;
		for (uint8_t j = 1; j <= MAX_RGB_LENGTH; j++)
		{
			if (!((str[j] >= '0' && str[j] <= '9') ||
				  (str[j] >= 'a' && str[j] <= 'f') ||
				  (str[j] >= 'A' && str[j] <= 'F')))
			{
				is_color_hex = FALSE;
				break;
			}
		}
	}

	if (i <= (MAX_STR_LENGTH - 1)
			&& (str[MAX_RGB_LENGTH + 1] == 'D') || str[MAX_RGB_LENGTH + 1] == 'd')
	{
		is_led_id_valid = TRUE;
		switch(str[MAX_RGB_LENGTH + 2])
		{
			case '6':	*led_id = 6; break;
			case '7':	*led_id = 7; break;
			case '8':	*led_id = 8; break;
			default:	is_led_id_valid = FALSE;
		}
	}

	if (i == (MAX_STR_LENGTH - 1))
	{
		uart_printstr(ANSI_RED);
		uart_printstr("Max length reached! Invalid input. Press Enter to retry.\r\n");
		uart_printstr(ANSI_RESET);
	}

	// Consume extra characters
	if (i == (MAX_STR_LENGTH - 1))
	{
		while (1)
		{
			if (c == '\r')
				break;
			c = uart_rx();
		}
	}

	if (is_color_hex == FALSE || is_led_id_valid == FALSE || i != 9) // '#' + 6 hex digits + Dx + \0
	{
		uart_printstr(ANSI_RED);
		uart_printstr("Invalid request!\r\n");
		uart_printstr(ANSI_RESET);
		return (INVALID);
	}

	// Parse the RGB hex value
	uint32_t rgb = 0;
	for (uint8_t j = 1; j <= 6; j++)
		rgb = (rgb << 4) | hex_char_to_val(str[j]);
	*b = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*r = (rgb >> 0) & 0xFF;

	return (STANDARD);
}

uint32_t	wheel(uint8_t pos)
{
	uint32_t	r, g, b;

	pos = 255 - pos;
	if (pos < 85)
	{
		r = 255 - pos * 3;
		g = 0;
		b = pos * 3;
	}
	else if (pos < 170)
	{
		pos = pos - 85;
		r = 0;
		g = pos * 3;
		b = 255 - pos * 3;
	}
	else
	{
		pos = pos - 170;
		r = pos * 3;
		g = 255 - pos * 3;
		b = 0;
	}

	r = (r << 0)  & 0x000000FF;
	g = (g << 8)  & 0x0000FF00;
	b = (b << 16) & 0x00FF0000;
	uint32_t color = BRIGHTNESS | r | g | b;
	return (color);
}


int main(void)
{
	char		color_str[MAX_STR_LENGTH];
	uint8_t		led_id;
	uint8_t		usr_request;
	uint32_t	color;
	uint32_t	r, g, b;

	uart_init();
	spi_master_init();
	while (1)
	{
		uart_printstr("Type color code and led ID (format #RRGGBBDX, with RRGGBB in hex. value and X = 6/7/8) and press Enter.\r\n");
		uart_printstr(" > ");
		usr_request = get_and_test_str(color_str, &r, &g, &b, &led_id);
		if (usr_request == STANDARD)
		{
			color = BRIGHTNESS | (r << 16) | (g << 8) | (b << 0);
			spi_tx_uint32(START_FRAME);
			for (uint8_t led = 6; led <= 8; led++)
			{
				if (led == led_id)
					spi_tx_uint32(color);
				else
					spi_tx_uint32(BRIGHTNESS | BLANK);
			}
			spi_tx_uint32(STOP_FRAME);

			uart_printstr(ANSI_GREEN);
			uart_printstr("Color applied!\r\n");
			uart_printstr(ANSI_RESET);
		}
		else if (usr_request == RAINBOW)
		{
			uart_printstr(ANSI_PURPLE);
			uart_printstr("Rainbow mode activated! Enjoy and wait completion.\r\n");
			uart_printstr(ANSI_RESET);
			for (uint8_t cycle = 0; cycle < 4; cycle++)
			{
				for (uint8_t pos = 0; pos < 255; pos++)
				{
					color = wheel(pos);
					spi_tx_uint32(START_FRAME);
					spi_tx_uint32(color); // D6
					spi_tx_uint32(color); // D7
					spi_tx_uint32(color); // D8
					spi_tx_uint32(STOP_FRAME);
					_delay_ms(10);
				}
			}
		}
		uart_printstr("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	}
}


// SK9822 Data Sheet : https://www.pololu.com/file/0J1234/sk9822_datasheet.pdf