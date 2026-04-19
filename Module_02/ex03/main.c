#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#define BAUD_RATE 115200

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

void uart_rx_interrupt_init()
{
	// Enable USART Receive Complete interrupt
	UCSR0B |= (1 << RXCIE0); // DS40002061B-p.212 - Writing this bit to one enables interrupt on the RXCn Flag
	// => USART Receive Complete interrupt if 1) RXCIEn = 1, 2) SREG |= (1 << 7), and 3) RXCn = 1
}

// cf. Example / DS40002061B-page 186
void uart_tx(unsigned char c)
{
	/// Wait for empty transmit buffer
	// DS40002061B-p.186 The function simply waits for the transmit buffer to be empty by checking the UDREn Flag, before loading it with
	// new data to be transmitted.
	while ((UCSR0A & (1<<UDRE0)) == 0)
	{}

	// Put data into buffer, sends the data
	UDR0 = c;
}

void uart_printstr(const char* str)
{
	while (*str)
		uart_tx(*str++);
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for USART Rx Complete
__attribute__((signal, used)) // ISR attribute: 'signal' ensures all registers (including SREG) are saved/restored; 'used' prevents compiler optimization from removing the ISR (called by hardware)
// void USART_RX_vect() // when using <avr/interrupt.h>
void __vector_18(void) // if not using <avr/interrupt.h>, we should use vector_18 (for USART Rx Complete)
{
	char c = UDR0; // Read the value directly in the Rx register

	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) 
		c ^= (1 << 5); // toggle bit 5 because A = 01000001 - a = 01100001
	if (c != 127) // if != DEL
		uart_tx(c);
	if (c == 127) // if = DEL
		uart_printstr("\b \b");
}

int main()
{
	uart_init();
	uart_rx_interrupt_init();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Satatus Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}

// DS40002061B-page 180 - Figure 20-1 - USART Block Diagram
// DS40002061B-page 181 - Figure 20-2 - Clock Generation Logic, Block Diagram

