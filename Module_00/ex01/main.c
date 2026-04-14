#include <avr/io.h>  // Required for AVR register definitions
// AVR registers are special memory locations in the microcontroller that control
// the behavior of its I/O ports, timers, interrupts, and other peripherals.
// cf. /usr/avr/include/avr/iom328p.h

int main()
{
	DDRB = DDRB | (1 << DDB0); // Set PB0 as output (DDB0 = 1)
	PORTB = PORTB | (1 << PORTB0);  // Turn on LED (PB0 = 1)

	while (1)
	{}

	return (0);
}

// Microcontrollers-ATmega328P_Datasheet.pdf

// --- DDRx (Data Direction Register) ---
// p.85 of 653 (14.2.1)
// Controls whether a pin is input (0) or output (1).
// Example for PORTB:
//   DDRB |= (1 << DDB0);  // Set PB0 as output
//   DDRB &= ~(1 << DDB0); // Set PB0 as input

// --- PORTx (Port Data Register) ---
// p.86 of 653 (14.2.2)
// If pin is output: Sets pin to 5V (1) or 0V (0).
// Example for PORTB:
//   PORTB |= (1 << PORTB0);  // Set PB0 to 5V (LED on)
//   PORTB &= ~(1 << PORTB0); // Set PB0 to 0V (LED off)

// --- Examples ---
// p.88 of 653 (14.2.4) => Cf. Examples for setting DDRx and PORTx
// DDRB &= ~(1 << DDB0);  // Clear the bit (set to 0)
// DDRB |= (1 << DDB0);   // Set the bit (set to 1)


// Microcontrollers-ATmega328P_Datasheet.pdf
// p.85 of 653 (14.2.1)
// Each port pin consists of three register bits: DDxn, PORTxn, and PINxn.
// The DDxn bit in the DDRx Register selects the direction of this pin. If DDxn is written logic one, Pxn is
// configured as an output pin. If DDxn is written logic zero, Pxn is configured as an input pin
// Data direction register – DDRx
// DDRB => corresponds to all PINB / PORTB
// DDRB = 1 => Output / 0 => Input
// DDB0 => corresponds to PINB0 / PORTB0
// p.86 of 653 (14.2.2)
// Port Data Register - PORTx
// PORTB = 1 => 5V / 0 => 0v
// PORTB0 => corresponds to PORTB0
// p.100 of 653 (14.14)
// Bit vs. Byte construction of PORTB and DDRB
// p.88 of 653 (14.2.4) => Cf. Examples for setting DDRx and PORTx
// DDRB &= ~(1 << DDB0);  // Clear the bit (set to 0)
// DDRB |= (1 << DDB0);   // Set the bit (set to 1)

// cat /usr/avr/include/avr/iom328p.h for reference for each register :

/* Registers and associated bit numbers */

// #define PINB _SFR_IO8(0x03)
// #define PINB0 0
// #define PINB1 1
// #define PINB2 2
// #define PINB3 3
// #define PINB4 4
// #define PINB5 5
// #define PINB6 6
// #define PINB7 7

// #define DDRB _SFR_IO8(0x04)
// #define DDB0 0
// #define DDB1 1
// #define DDB2 2+
// #define DDB3 3
// #define DDB4 4
// #define DDB5 5
// #define DDB6 6
// #define DDB7 7

// #define PORTB _SFR_IO8(0x05)
// #define PORTB0 0
// #define PORTB1 1
// #define PORTB2 2
// #define PORTB3 3
// #define PORTB4 4
// #define PORTB5 5
// #define PORTB6 6
// #define PORTB7 7
