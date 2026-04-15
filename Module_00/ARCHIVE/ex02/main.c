#include <avr/io.h>

int main()
{
	DDRB |= (1 << DDB0);  // Set DDB0 to 1 (PB0 (i.e. LED D1) as output)
	DDRD &= ~(1 << DDD2);  // Set DDD2 to 0 (PD2 (i.e. switch SW1) as input)
	PORTD |= (1 << PORTD2);  // Set PORTD2 to 1 (enable pull-up resistor)

	while (1)
	{
		if (!(PIND & (1 << PIND2))) // Check if PD2 is low (button pressed)
			PORTB |= (1 << PORTB0); // Turn on LED (PB0 = 1)
		else
			PORTB &= ~(1 << PORTB0); // Turn off LED (PB0 = 0)
	}

	return (0);
}

// Microcontrollers-ATmega328P_Datasheet.pdf

// --- DDRx (Data Direction Register) ---
// p.85 of 653 (14.2.1)
// Controls whether a pin is input (0) or output (1).
// DDRB => Controls PORTB (PB0-PB7)
// DDRB = 1 => Output / 0 => Input
// Example:
//   DDRB |= (1 << DDB0);  // Set PB0 as output
//   DDRB &= ~(1 << DDB0); // Set PB0 as input

// --- PORTx (Port Data Register) ---
// p.86 of 653 (14.2.2)
// If pin is output: Sets pin to 5V (1) or 0V (0).
// If pin is input: Enables (1) or disables (0) internal pull-up resistor.
// PORTB => Controls output values or pull-ups for PORTB.
// Example:
//   PORTB |= (1 << PORTB0);  // Set PB0 to 5V (output) or enable pull-up (input)
//   PORTB &= ~(1 << PORTB0); // Set PB0 to 0V (output) or disable pull-up (input)

// --- PINx (Port Input Pins) ---
// p.87 of 653 (14.2.3)
// Reads the current state of a pin (high/low).
// PINB => Reads the state of PORTB pins.
// Example:
//   if (PINB & (1 << PINB0))  // Check if PB0 is high
//   if (!(PIND & (1 << PIND2))) // Check if PD2 is low (button pressed)

// Internal pull-up resistor (Rpu)
// p.323 of 653 (Table 30-1)
// I/O Pin Pull-up Resistor 20k to 50k Ohm

// Electrical schematic for Rpu
// p. 84 of 653 (Figure 14-1)
// https://learn.sparkfun.com/tutorials/pull-up-resistors/all

// PD2 (switch SW1) pull-up resistor
// 42 PCB Schematic => 10kOhm

// Electrical schematic for PD2 (switch SW1) pull-up resistor
// 42 PCB Schematic
// https://learn.sparkfun.com/tutorials/pull-up-resistors/all

