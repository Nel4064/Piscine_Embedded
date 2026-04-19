#include <avr/io.h>
#include <util/delay.h>
// #include <avr/interrupt.h>

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

	// Enable transmitter (DS40002061B-page 201)
	UCSR0B |= (1 << TXEN0);

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

void timer1_compa_init()
{
	// Set Clock Select (after Prescaler) to 101 = 1024 (Vs. 1/8/64/256)
	TCCR1B |= (1 << CS12); // p. 143 clkI/O/1024 (From prescaler)
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B |= (1 << CS10); // idem above

	// Set mode of operation to Toggle OC1A on Compare Match
	// with OC1A = Timer/Counter1 Output Compare Match A Output
	TCCR1A |= (1 << COM1A0); // p.91 + p.162 Toggle OC1A on Compare Match // overrides the normal port functionality of the I/O pin
	TCCR1A &= ~(1 << COM1A1); // idem above
	
	// Set mode of operation to CTC (avoid us to reinitialise OCF1A bit in TIFR1 register)
	// with CTC = Clear Timer on Compare match
	TCCR1B &= ~(1 << WGM13); // idem below
	TCCR1B |= (1 << WGM12); // p. 141 + 142 Timer/Counter Mode of Operation set to CTC 
	TCCR1A &= ~(1 << WGM11); // idem above
	TCCR1A &= ~(1 << WGM10); // idem above

	// Set TOP value (TOP = OCR1A because CTC mode with WGM13:10 = 0b0100)) / p.121 + 122 Figure 16-1 and Definitions of TOP value and role in comparison
	OCR1A = F_CPU/(1024/2) - 1; // = 7811 value set in OCR1AH and OCR1AL
	// OCR1AH = 0b00011110;
	// OCR1AL = 0b10000011;

	// Set flag OCF1A as an interrupt trigger enabled for TIMER1_COMPA fonction execution
	TIMSK1 |= (1 << OCIE1A); // p. 145 When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the
							 // Timer/Counter1 Output Compare A Match interrupt is enabled. The corresponding Interrupt Vecto

	// Re-start clock at 0 after all the setting above (not stricly necessary... but why not!)
	TCNT1 = 0; // p.122/123 Example of setting TCNT1
	// TCNT1H = 0b00000000;
	// TCNT1L = 0b00000000;
}

// cf. Example / DS40002061B-page 186
void uart_tx(const char c)
{
	/// Wait for empty transmit buffer
	// DS40002061B-p.186 The function simply waits for the transmit buffer to be empty by checking the UDREn Flag, before loading it with
	// new data to be transmitted.
	while ((UCSR0A & (1 << UDRE0)) == 0)
	{}

	// Put data into buffer, sends the data
	UDR0 = c;
}

void uart_printstr(const char* str)
{
	while (*str)
		uart_tx(*str++);
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for Timer1 Compare A
__attribute__((signal, used)) // ISR attribute: 'signal' ensures all registers (including SREG) are saved/restored; 'used' prevents compiler optimization from removing the ISR (called by hardware)
// void TIMER1_COMPA_vect() // when using <avr/interrupt.h>
void __vector_11(void) // if not using <avr/interrupt.h>, we should use vector_11 (the vector number for Timer1 Compare A).
{
	uart_printstr("Hello World!\r\n");
}

int main(void)
{
	uart_init();
	timer1_compa_init();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Satatus Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}

// DS40002061B-page 180 - Figure 20-1 - USART Block Diagram
// DS40002061B-page 181 - Figure 20-2 - Clock Generation Logic, Block Diagram

// Interrupts and Signals in the AVR-GCC environment
// https://ccrma.stanford.edu/courses/250a-fall-2005/docs/avr-libc-user-manual-1.2.5/group__avr__interrupts.html
// => How to define and use Interrupt Service Routines (ISRs) in AVR-GCC, including attributes like signal.
// => It covers ISR naming conventions, register preservation, and the role of the avr/interrupt.h library for simplified ISR declarations.
// __attribute__((signal)) : Saves all registers (including SREG) before entering the ISR and restores them afterward.
// __attribute__((interrupt)) : Saves only registers used in the ISR (optimized but riskier). Rarely used in AVR. More common in other architectures (e.g., ARM).


// ISR (Interrupt Service Routine) = A function executed in response to a hardware or software interrupt.
// => temporarly suspends the main program to handle urgent tasks (e.g., timers, UART, external events).
