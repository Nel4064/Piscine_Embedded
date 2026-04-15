#include <avr/io.h>

#ifndef F_CPU
 define F_CPU 16000000UL;
#endif

int main()
{
	DDRB |= (1 << DDB1); // PB1/LED D2 = OUTPUT (Note : PB1 might be set to OC1A)

	// Enable Timer/Counter1
	PRR &= ~(1 << PRTIM1);  // p.54 + p.150 PRTIM1 must be written to zero to enable Timer/Counter1 module
	
	// Set mode of operation to Toggle OC1A on Compare Match
	// with OC1A = Timer/Counter1 Output Compare Match A Output
	TCCR1A |= (1 << COM1A0); // p.91 + p.162 Toggle OC1A on Compare Match // overrides the normal port functionality of the I/O pin
	TCCR1A &= ~(1 << COM1A1); // idem above
	
	// Set mode of operation to CTC (avoid us to reinitialise TOV2 by setting TIFR2)
	// with CTC = Clear Timer on Compare match
	TCCR1B &= ~(1 << WGM13); // idem below
	TCCR1B |= (1 << WGM12); // p. 141 + 142 Timer/Counter Mode of Operation set to CTC 
	TCCR1A &= ~(1 << WGM11); // idem above
	TCCR1A &= ~(1 << WGM10); // idem above

	OCR1A = F_CPU/(1024*2) - 1; // = 7811 value set in OCR1AH and OCR1AL
	// OCR1AH = 0b00011110;
	// OCR1AL = 0b10000011;

	// Start clock with prescaler 101 = 1024 (vs. 1/8/64/256)
	TCCR1B |= (1 << CS12); // p. 143 clkI/O/1024 (From prescaler)
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B |= (1 << CS10); // idem above

	while (1)
	{
	}

	return (0);
}

// TIMERS
// https://www.locoduino.org/spip.php?article84
// https://www.locoduino.org/spip.php?article89

// Register for timer :
// Si le registre du timer comporte 8 bits, il est alors capable de compter de 0 à 255 
// (en hexadécimal, de 00 à FF). Lorsqu’il arrive à 255 (FF), un coup d’horloge supplémentaire devrait 
// le faire passer à 256 (soit 100 en hexadécimal), ce qui n’est pas possible puisque le registre n’a que 8 bits. Le registre passe donc à 0 
// Prescaler :
// il est possible de diviser cette fréquence d’horloge (16Mhz) grâce à des circuits internes
// au microcontrôleur appelés prédiviseur (= prescaler). On peut alors diviser la fréquence de base (16 MHz) 
// par 8, 32, 64, 128, 256 ou 1024 

// 
//  * ATmega328P Timer Registers Overview:
//  *
//  * +---------+---------+---------+-------------------------------+
//  * | Timer 0 | Timer 1  | Timer 2 | Role                         |
//  * +---------+---------+---------+-------------------------------+
//  * | TCNT0   | TCNT1L  | TCNT2   | Timer/Counter (bits 0-7)      |
//  * |         | TCNT1H  |         | Timer/Counter (bits 8-15)     |
//  * |---------+---------+---------+-------------------------------|
//  * | TCCR0A  | TCCR1A  | TCCR2A  | Control Register A            |
//  * | TCCR0B  | TCCR1B  | TCCR2B  | Control Register B            |
//  * |         | TCCR1C  |         | Control Register C            |
//  * |---------+---------+---------+-------------------------------|
//  * | OCR0A   | OCR1AL  | OCR2A   | Output Compare Register (0-7) |
//  * |         | OCR1AH  |         | Output Compare Register (8-15)|
//  * | OCR0B   | OCR1BL  | OCR2B   | Output Compare Register (0-7) |
//  * |         | OCR1BH  |         | Output Compare Register (8-15)|
//  * |---------+---------+---------+-------------------------------|
//  * |         | ICR1L   |         | Input Capture Register (0-7)  |
//  * |         | ICR1H   |         | Input Capture Register (8-15) |
//  * |---------+---------+---------+-------------------------------|
//  * | TIMSK0  | TIMSK1  | TIMSK2  | Interrupt Mask Register       |
//  * | TIFR0   | TIFR1   | TIFR2   | Interrupt Flag Register       |
//  * +---------+---------+---------+-------------------------------+
//

// TCNT Timer/Counter (Register)
// TCCR Timer/Counter Control Register
// OCR Output Compare Register
// ICR Input Capture Register
// TIMSK Timer/Counter Interrupt Mask Register
// TIFR Timer/Counter Interrupt Flag Register

// Timer clock = F_CPU / prescaler = 16,000,000 / 1024 = 15,625 Hz
// Counts per toggle = 15,625 × 0.5s = 7,812.5
// OCR1A = 7,812 - 1 = 7,811  (timer counts 0 to OCR1A, so subtract 1)