#include "uart.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#ifndef BAUD_RATE
 #define BAUD_RATE	115200
#endif

// cf. Example / DS40002061B-page 185
void uart_init()
{
	// Define UBRR0 register value
	uint16_t ubrr;
	ubrr = 16; // (cf. DS40002061B-p.199, table 20-7, 16MHz/115.2k/U2X0 = 1)

	// Set baud rate (DS40002061B-page 204)
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	// Enable transmitter and receiver (DS40002061B-page 201)
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);

	// Set USART mode to Asynchronous USART	(DS40002061B-p.202)
	UCSR0C &= ~(1 << UMSEL01);
	UCSR0C &= ~(1 << UMSEL00);

	// Set frame format (DS40002061B-p.202/203)
	// no parity
	UCSR0C &= ~(1 << UPM01);
	UCSR0C &= ~(1 << UPM00);
	// 1-stop bit
	UCSR0C &= ~(1 << USBS0);
	// 8-bit data
	UCSR0C &= ~(1 << UCSZ02);
	UCSR0C |= (1 << UCSZ01);
	UCSR0C |= (1 << UCSZ00);

	// Set Double USART Transmission Speed (DS40002061B-p.199, Table 20-7, 2.1% error vs. 3.5%)
	// i.e. Sample/Rx clock frequency = 1/8 * Tx clock frequency
	UCSR0A |= (1 << U2X0); // DS40002061B-p.200/201 U2Xn bit and register and p.181 - Figure 20-2.
}

// cf. Example / DS40002061B-page 186
void uart_tx(unsigned char c)
{
	/// Wait for empty transmit buffer, , i.e UDRE0 flag = 1
	// DS40002061B-p.186 The function simply waits for the transmit buffer to be empty by checking the UDREn Flag, before loading it with
	// new data to be transmitted.
	while ((UCSR0A & (1<<UDRE0)) == 0)
	{}

	// Put data into buffer, sends the data
	UDR0 = c;
}

void uart_tx_uint10(uint16_t value)
{
	char buffer[4]; // Max 4 digits (1024)

	buffer[0] = (value / 1000) % 10 + '0';
	buffer[1] = (value / 100) % 10 + '0';
	buffer[2] = (value / 10) % 10 + '0';
	buffer[3] = value % 10 + '0';

	// Replace leading zero(s) with space (but keep last digit)
	uint8_t i = 0;
	while (i < 3 && buffer[i] == '0')
		buffer[i++] = ' ';

	for (uint8_t i = 0; i < 4; i++)
		uart_tx(buffer[i]);
}

void uart_printstr(const char* str)
{
	while (*str)
		uart_tx(*str++);
}

void uart_display_status(uint8_t status_code)
{
	uart_printstr("[I2C Status: ");
	uart_printstr(i2c_status_desc(status_code));
	uart_tx(']');
	uart_tx('\r');
	uart_tx('\n');
}

// cf. Example / DS40002061B-page 189
char uart_rx(void)
{
	// Wait for data to be received, i.e RXC0 flag = 1
	while ((UCSR0A & (1<<RXC0)) == 0)
	{}

	// Get and return received data from buffer
	return (UDR0);
}