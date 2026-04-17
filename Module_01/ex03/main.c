#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 define F_CPU 16000000UL;
#endif

int main()
{
	uint32_t duty = 10;

	DDRB |= (1 << DDB1); // PB1/LED D2 = OUTPUT (Note PB1 : Can be set by OC1A, critical here for this implementation)

	// Buttons (PD2/SW1, PD4/SW2) = INPUT + ENABLED
	DDRD &= ~((1 << PD2) | (1 << PD4));
	PORTD |= (1 << PD2) | (1 << PD4);

	// 0 = not pressed, 1 = pressed
	uint8_t	was_button_SW1_pressed = 0;
	uint8_t	was_button_SW2_pressed = 0;

	// Enable Timer/Counter1
	PRR &= ~(1 << PRTIM1);  // p.54 + p.120 PRTIM1 must be written to zero to enable Timer/Counter1 module
	
	// // Reset Prescaler (=> no prescaler division)
	// GTCCR |= (1 << PSRSYNC); // p.149 When this bit is one, Timer/Counter1 and Timer/Counter0 prescaler will be Reset
	//  						// p.147 the prescaler is not affected by the Timer/Counter’s clock select, but the prescaler will have implications for situations where a prescaled clock is used

	// // As an alternative to Prescaler Reset, set prescaler division to 1
	// CLKPR |= (1 << CLKPCE); // p.46 CLKPCE bit must be written to logic one to enable change of the CLKPS bits
	// CLKPR &= ~((1 << CLPKS0) | (1 << CLPKS1) | (1 << CLPKS2) | (1 << CLPKS3)) // p.47 Set Clock Division Factor = 1
	
	// Set Clock Select (after Prescaler) to 101 = 1024 (Vs. 1/8/64/256)
	TCCR1B |= (1 << CS12); // p. 143 clkI/O/1024 (From prescaler)
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B |= (1 << CS10); // idem above

	// Set mode of operation to Fast PWM, TOP = ICR1 (WGM13:10 = 0b1110)
	// with PWM = Pulse Width Modulation
	TCCR1B |= (1 << WGM13);
	TCCR1B |= (1 << WGM12);
	TCCR1A |= (1 << WGM11);
	TCCR1A &= ~(1 << WGM10);

	// Set mode of operation to non inverting mode
	// => p.140 Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	// => p.132 In non-inverting Compare Output mode, the Output Compare (OC1x) is cleared on the compare match between TCNT1 and OCR1x, and set at BOTTOM.
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// Set ICR1 to TOP (ICR1 = TOP because of mode (WGM13:10 = 0b1110))
	ICR1 = (F_CPU/(1024*1) - 1);
	// Note : p.127 Waveform Generation mode (WGM13:0) bits must be set before the TOP value can be written to the ICR1 Register. 

	// Set OCR1A / p.121 + 122 Figure 16-1 and Definitions of TOP value and role in comparison
	OCR1A = (uint32_t)ICR1 * duty / 100; // for ICR1 / 10 => 10% duty cycle 

	// Re-start clock at 0 after all the setting above (not stricly necessary... but why not!)
	TCNT1 = 0; // p.122/123 Example of setting TCNT1
	// TCNT1H = 0b00000000;
	// TCNT1L = 0b00000000;

	while (1)
	{
		uint8_t	is_button_SW1_pressed = !(PIND & (1 << PIND2));
		uint8_t	is_button_SW2_pressed = !(PIND & (1 << PIND4));

		if (!was_button_SW1_pressed && is_button_SW1_pressed)
		{
			_delay_ms(20);
			is_button_SW1_pressed = !(PIND & (1 << PIND2));
			if (is_button_SW1_pressed && duty <= 90)
				duty += 10;
		}

		if (!was_button_SW2_pressed && is_button_SW2_pressed)
		{
			_delay_ms(20);
			is_button_SW2_pressed = !(PIND & (1 << PIND4));
			if (is_button_SW2_pressed && duty >= 10)
				duty -= 10;
		}
		OCR1A = (uint32_t)ICR1 * duty / 100; 
		was_button_SW1_pressed = is_button_SW1_pressed;
		was_button_SW2_pressed = is_button_SW2_pressed;
	}
}

// // Other key information
// p.121 + 122 Figure 16-1 and Definitions of TOP value and role in comparison


// // Alternative approach with PWM, Phase Correct (WGM13:10 = 0b1010)
// int main()
// {
// 	DDRB |= (1 << DDB1); // PB1/LED D2 = OUTPUT (Note : PB1 might be set to OC1A)

// 	// Enable Timer/Counter1
// 	PRR &= ~(1 << PRTIM1);  // p.54 + p.120 PRTIM1 must be written to zero to enable Timer/Counter1 module
	
// 	// Reset Prescaler (=> no prescaler division)
// 	GTCCR |= (1 << PSRSYNC); // p.149 When this bit is one, Timer/Counter1 and Timer/Counter0 prescaler will be Reset
// 	 						// p.147 the prescaler is not affected by the Timer/Counter’s clock select, but the prescaler will have implications for situations where a prescaled clock is used

// 	// // As an alternative to Prescaler Reset, set prescaler division to 1
// 	// CLKPR |= (1 << CLKPCE); // p.46 CLKPCE bit must be written to logic one to enable change of the CLKPS bits
// 	// CLKPR &= ~((1 << CLPKS0) | (1 << CLPKS1) | (1 << CLPKS2) | (1 << CLPKS3)) // p.47 Set Clock Division Factor = 1
	
// 	// Set Clock Select (after Prescaler) to 101 = 1024 (Vs. 1/8/64/256)
// 	TCCR1B |= (1 << CS12); // p. 143 clkI/O/1024 (From prescaler)
// 	TCCR1B &= ~(1 << CS11); // idem above
// 	TCCR1B |= (1 << CS10); // idem above

// 	// Set mode of operation to PWM, Phase Correct (WGM13:10 = 0b1010)
// 	// with PWM = Pulse Width Modulation
// 	TCCR1B |= (1 << WGM13); // See above
// 	TCCR1B &= ~(1 << WGM12); // See above
// 	TCCR1A |= (1 << WGM11); // See above
// 	TCCR1A &= ~(1 << WGM10); // See above

// 	// Set mode of operation
// 	// => Clear OC1A/OC1B on Compare Match when up-
// 	// counting. Set OC1A/OC1B on Compare Match when
// 	// downcounting.
// 	TCCR1A &= ~(1 << COM1A0);
// 	TCCR1A |= (1 << COM1A1); // idem above

// 	// Set ICR1 to TOP (ICR1 = TOP because of mode (WGM13:10 = 0b1110))
// 	ICR1 = (F_CPU/(1024*1)/2 - 1);
// 	// Note : p.127 Waveform Generation mode (WGM13:0) bits must be set before the TOP value can be written to the ICR1 Register. 

// 	// Set OCR1A for 10% duty cycle / p.121 + 122 Figure 16-1 and Definitions of TOP value and role in comparison
// 	OCR1A = ICR1 / 10;

// 	// Re-start clock at 0 after all the setting above (not stricly necessary... but why not!)
// 	TCNT1 = 0; // p.122/123 Example of setting TCNT1
// 	// TCNT1H = 0b00000000;
// 	// TCNT1L = 0b00000000;

// 	while (1)
// 	{
// 	}
// }