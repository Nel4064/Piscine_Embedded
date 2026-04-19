#include <avr/io.h>
#include <util/delay.h>
#include "string.c"
#include "uart.c"
#include "rgb_light.c"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#ifndef BAUD_RATE
 #define BAUD_RATE	115200
#endif

#define MAX_LENGTH	10
#define TRUE  		1
#define FALSE 		0

void get_and_test_str(char* str, uint8_t length)
{
	char c;
	uint8_t i = 0;
	bool is_color_hex = FALSE;

	while (i < length - 1)
	{
		c = uart_rx();
		if (c == 127) // if = DEL (backspace)
		{
			if (i > 0)
			{
				uart_printstr("\b \b");
				i--;
			}
			continue;
		}
		if (c == '\r')
			break;
		str[i] = c;
		uart_tx(c);
		i++;
	}
	str[i] = '\0';
	uart_printstr("\r\n");
	if (str[0] == '#' && i <= length - 1) // '#' + 6 hex digits
	{
		is_color_hex = TRUE;
		for (uint8_t j = 1; j <= 6; j++)
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

	if (i == length - 1)
		uart_printstr("Max length reached! Invalid input. Press Enter to retry.\r\n");

	// Consume extra characters
	if (i == length - 1 && c != '\r')
	{
		while (1)
		{
			c = uart_rx();
			if (c == '\r')
				break;
		}
	}

	if (!is_color_hex || i != 7) // '#' + 6 hex digits + \0
		uart_printstr("Invalid color code!\r\n");
}

int main(void)
{
	char color_str[MAX_LENGTH];
	uint8_t r = 0xFF, g = 0xFF, b = 0xFF;

	init_led();
	init_rgb();
	init_uart();
	set_rgb(r, g, b);
	while (1)
	{
		uart_printstr("Type color code (format #XXXXXX, with X hex. value) and press Enter.\r\n> ");
		get_and_test_str(color_str, MAX_LENGTH);
		r = (hex_char_to_val(color_str[1]) << 4) | hex_char_to_val(color_str[2]);
		g = (hex_char_to_val(color_str[3]) << 4) | hex_char_to_val(color_str[4]);
		b = (hex_char_to_val(color_str[5]) << 4) | hex_char_to_val(color_str[6]);
		set_rgb(r, g, b);
		uart_printstr("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	}
}

// PWM Frequency Table for Timer0/Timer2 (8-bit Fast PWM, TOP=0xFF)
// Equation: F_PWM = F_CPU / (prescaler * 256)
// F_CPU = 16 MHz for ATmega328P at 16 MHz clock

// Prescaler | CS Bits (CS02:CS00 or CS22:CS20) | Frequency (Hz) | Notes
// ----------|----------------------------------|----------------|-------
//     1     |            0b001                 |     62,500     | > 2 kHz => Ideal
//     8     |            0b010                 |      7,812     | > 2 kHz => Ideal
//    64     |            0b011                 |        976     | < 100 Hz => Flickering visible
//   256     |            0b100                 |        244     | < 100 Hz => Flickering visible
//  1024     |            0b101                 |         61     | < 100 Hz => Flickering visible

// Source : https://www.ti.com/document-viewer/lit/html/SSZTAY6
