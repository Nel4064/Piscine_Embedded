#include <avr/io.h>
#include "uart.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define EEPROM_SIZE				1024	
#define HEXDUMP_BYTES_PER_LINE	16

typedef enum e_eeprom_error_code
{
	EEPROM_ERROR_NONE = 0,
	EEPROM_INVALID_ADDR,
}	t_eeprom_error_code;

// // Mock EEPROM read function
// uint8_t eeprom_read_byte(uint16_t address, uint8_t *data)
// {
// 	// Mock data: Fill with a repeating pattern for testing
// 	static const uint8_t mock_data[] =
// 	{
// 		0x57, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x63, 0x61, 0x6c, 0x6c,
// 		0x20, 0x61, 0x20, 0x6d, 0x65, 0x64, 0x69, 0x74, 0x61, 0x74, 0x69, 0x6e, 0x67, 0x20, 0x65, 0x6c,
// 		0x65, 0x63, 0x74, 0x72, 0x69, 0x63, 0x69, 0x61, 0x6e, 0x3f, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00
// 	};
// 	*data = mock_data[address % sizeof(mock_data)];
// 	return (EEPROM_OK);
// }

// DS40002061B-p.35 / Example EEPROM_read
uint8_t eeprom_read_byte(uint16_t address, uint8_t *data)
{
	if (address > EEPROM_SIZE)
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

int main(void)
{
	uint8_t data;

	uart_init();

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
			eeprom_read_byte(addr + i, &data);
			uart_tx_hex(data);
			uart_tx(' ');
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


// Ref. EEPROM Read/Write, ATmega328P Programming #10 AVR microcontroller with Atmel Studio 
// https://www.youtube.com/watch?v=q8C94CHXb6A