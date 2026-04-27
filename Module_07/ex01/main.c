#include <avr/io.h>
#include "uart.h"
#include "delay_us_and_ms.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define EEPROM_SIZE				1024	
#define HEXDUMP_BYTES_PER_LINE	16
#define MAX_STR_LENGTH			10
#define TRUE  					1
#define FALSE 					0

typedef enum e_eeprom_error_code
{
	EEPROM_ERROR_NONE = 0,
	EEPROM_INVALID_ADDR
}	t_eeprom_error_code;

typedef enum e_parsing_error_code
{
	PARSING_ERROR_NONE = 0,
	PARSING_INVALID_CHAR,
	PARSING_TOO_MANY_CHAR
}	t_parsing_error_code;

// DS40002061B-p.35 / Example EEPROM_read
uint8_t eeprom_read_byte(uint16_t address, uint8_t *data)
{
	if (address >= EEPROM_SIZE)
		return (EEPROM_INVALID_ADDR);

	// DS40002061B-p.31 Wait for completion of previous write
	while(EECR & (1 << EEPE)) // The user software can poll this bit and wait for a zero before writing the next byt
	{}

	// DS40002061B-p.31 Set up address register
	EEARH = (address & 0x0300) >> 8;
	EEARL = (address & 0x00FF) >> 0;

	// DS40002061B-p.33 Start eeprom read by writing EERE 
	EECR |= (1 << EERE); // When the correct address is set up in the EEAR Register, the EERE bit must be written to a logic one to trigger the EEPROM read. 

	// DS40002061B-p.31 Return data from Data Register
	*data = EEDR; // For the EEPROM read operation, the EEDR contains the data read out from the EEPROM at the address given by EEAR

	return (EEPROM_ERROR_NONE);
}

// DS40002061B-p.34 / Example EEPROM_write
uint8_t eeprom_write_byte(uint16_t address, uint8_t data)
{
	if (address >= EEPROM_SIZE)
		return (EEPROM_INVALID_ADDR);

	// DS40002061B-p.31 Wait for completion of previous write
	while(EECR & (1 << EEPE)) // The user software can poll this bit and wait for a zero before writing the next byt
	{}

	// DS40002061B-p.31 Set up address register
	EEARH = (address & 0x0300) >> 8;
	EEARL = (address & 0x00FF) >> 0;

	// DS40002061B-p.31 Write data to Data Register
	EEDR = data; // For the EEPROM write operation, the EEDR Register contains the data to be written to the EEPROM in the address given by the EEAR Register. 

	// DS40002061B-p.32 Write logical one to EEMPE
	EECR |= (1 << EEMPE) ; // The EEMPE bit determines whether setting EEPE to one causes the EEPROM to be written.

	// DS40002061B-p.32 Start eeprom write by setting EEPE
	EECR |= (1 << EEPE); // When address and data are correctly set up, the EEPE bit must be written to one to write the value into the EEPROM.

	return (EEPROM_ERROR_NONE);
}

// void	eeprom_hexdump(void)
// {
// 	uint8_t data;

// 	for (uint16_t addr = 0; addr < EEPROM_SIZE ; addr += HEXDUMP_BYTES_PER_LINE)
// 	{
// 		// Display EEPROM Address / dec
// 		uart_tx_dec_uint10(addr);
// 		uart_tx(' ');
// 		uart_tx('-');
// 		uart_tx(' ');

// 		// Display EEPROM Data / hex
// 		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
// 		{
// 			eeprom_read_byte(addr + i, &data);
// 			uart_tx_hex(data);
// 			uart_tx(' ');
// 		}
// 		uart_tx('|');

// 		// Display EEPROM Data / unsigned char
// 		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
// 		{
// 			eeprom_read_byte(addr + i, &data);
// 			if (data >= ' ' && data <= 127)
// 				uart_tx(data);
// 			else
// 				uart_tx('.');
// 		}
// 		uart_tx('|');
// 		uart_tx('\r');
// 		uart_tx('\n');
// 	}
// }

void	eeprom_hexdump_with_highlight(uint16_t highlight_addr)
{
	uint8_t data;

	for (uint16_t addr = 0; addr < EEPROM_SIZE ; addr += HEXDUMP_BYTES_PER_LINE)
	{
		// Display EEPROM Address / dec
		uart_tx_dec_uint10(addr);
		uart_tx(' ');
		uart_tx('-');
		uart_tx(' ');

		// Display EEPROM Data / hex
		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			uint16_t current_addr = addr + i;
			eeprom_read_byte(current_addr, &data);
			if (current_addr == highlight_addr)
			{
				uart_printstr("\033[31m");  // Start red
				uart_tx_hex(data);
				uart_printstr("\033[0m");  // Reset color
			}
			else
				uart_tx_hex(data);
		}
		uart_tx('|');

		// Display EEPROM Data / unsigned char
		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			eeprom_read_byte(addr + i, &data);
			if (data >= ' ' && data <= 127)
				uart_tx(data);
			else
				uart_tx('.');
		}
		uart_tx('|');
		uart_tx('\r');
		uart_tx('\n');
	}
}

uint8_t	get_and_test_str(char* str, uint8_t length, uint16_t *addr, uint8_t *data)
{
	char	c;
	uint8_t	i = 0;
	bool	is_addr_dec = FALSE;
	bool	is_data_hex = FALSE;

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

	// Parse address (decimal)
	*addr = 0;
	uint8_t j = 0;
	while (j < i && str[j] != ' ')
	{
		if (str[j] >= '0' && str[j] <= '9')
			*addr = *addr * 10 + (str[j] - '0');
		else
		{
			is_addr_dec = FALSE;
			break ;
		}
		j++;
	}
	is_addr_dec = (j > 0);

	// Parse data (hex)
	*data = 0;
	if (j < i && str[j] == ' ')
	{
		j++;
		uint8_t hex_digits = 0;
		while (j < i)
		{
			if ((str[j] >= '0' && str[j] <= '9'))
				*data = (*data << 4) | (str[j] - '0');
			else if ((str[j] >= 'a' && str[j] <= 'f'))
				*data = (*data << 4) | (str[j] - 'a' + 10);
			else if ((str[j] >= 'A' && str[j] <= 'F'))
				*data = (*data << 4) | (str[j] - 'A' + 10);
			else
			{
				is_data_hex = FALSE;
				break ;
			}
			j++;
			hex_digits++;
		}
		is_data_hex = (hex_digits == 2);
	}

	if (i == length - 1)
	{
		uart_printstr("Max length reached! Invalid input. Press Enter to retry.\r\n");
		// Consume extra characters
		while (uart_rx() != '\r');
		return(PARSING_TOO_MANY_CHAR);
	}

	if (!is_addr_dec || !is_data_hex)
	{
		uart_printstr("Invalid input! Address must be 0-1023 (dec) and data must be 2-digit hex.\r\n");
		return(PARSING_INVALID_CHAR);
	}

	return(PARSING_ERROR_NONE);
}

int main(void)
{
	char		addr_and_data_str[MAX_STR_LENGTH];
	uint16_t	address;
	uint8_t		data;
	uint8_t		parsing_err;
	uint8_t		eeprom_err;

	uart_init();
	init_timer_for_delay();

	while (1)
	{
		uart_printstr("Type address (must be 0-1023 (dec)), 'space' and data (2-digit hex) and press Enter.\r\n> ");
		parsing_err = get_and_test_str(addr_and_data_str, MAX_STR_LENGTH, &address, &data);
		eeprom_err = eeprom_write_byte(address, data);
		if (eeprom_err == EEPROM_INVALID_ADDR)
			uart_printstr("Invalid address! Address must be 0-1023 (dec).\r\n");
		else if (parsing_err != PARSING_ERROR_NONE)
			;
		else
			eeprom_hexdump_with_highlight(address);
		uart_printstr("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		delay_ms(2);
	}
}

// Ref. EEPROM Read/Write, ATmega328P Programming #10 AVR microcontroller with Atmel Studio 
// https://www.youtube.com/watch?v=q8C94CHXb6A

