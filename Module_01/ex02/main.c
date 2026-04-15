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
