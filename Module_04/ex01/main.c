#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

void	led_d2_init(void)
{
	DDRB |= (1 << DDB1); // p.101 Set PB1/OC1A (= LED_D2) as OUTPUT
}

void	timer0_interrupt_init(void)
{
	// Set Normal mode for Timer/Counter Mode of Operation (WGM02:WGM00 = 0b0000)
	TCCR0B &= (1 << WGM02); // p. 115 Timer/Counter Mode of Operation
	TCCR0A &= (1 << WGM01); // idem above
	TCCR0A &= (1 << WGM00); // idem above
	// TOP = 0xFF

	// Set Normal port operation, OC0A disconnected (no use / no LED, etc.)
	TCCR0A &= ~(1 << COM0A1); // p.113
	TCCR0A &= ~(1 << COM0A0); // idem above

	// Enable interrupt on Timer/Counter0 overflow (TOP = 0xFF)
	TIMSK0 |= (1 << TOIE0); // p.118 Timer/Counter0, Overflow Interrupt Enable

	// Set Clock Select to 0b011 = 64 (Vs. 1/8/64/256/1028)
	TCCR0B &= ~(1 << CS02); // p.143 clkI/O
	TCCR0B |= (1 << CS01); // idem above
	TCCR0B |= (1 << CS00); // idem above
	// F_Overflow = F_CPU / (prescaler * (1 + TOP)) = 16,000,000 / (64 * 256) = 976.5625 Hz

	// Reset/Initialize Timer 0 counter
	TCNT0 = 0;
}

void	timer1_pwm_init(void)
{
	// Set Fast PWM mode with TOP at ICR1 (Mode 14)
	TCCR1B |= (1 << WGM13); // p. 141 + 142 Timer/Counter Mode of Operation
	TCCR1B |= (1 << WGM12); // idem above
	TCCR1A |= (1 << WGM11); // idem above
	TCCR1A &= ~(1 << WGM10); // idem above
	
	// Set non-inverting mode for OC1A (PB1)
	// with OC1A = Timer/Counter1 Output Compare Match A Output
	TCCR1A |= (1 << COM1A1); // p.91 + p.162
	TCCR1A &= ~(1 << COM1A0); // idem above
	
	// Set TOP value (TOP = ICR1) / p.121 + 122 Figure 16-1 and Definitions of TOP value and role in comparison
	// F_PWM = F_CPU / (prescaler * (1 + TOP))
	ICR1 = (uint16_t)((uint32_t)F_CPU/(2000UL * 64UL)) - 1; // = 124, for F_PWM = 2kHtz
	OCR1A = 0;

	// Set Clock Select to 0b010 = 64 (Vs. 1/8/64/256)
	TCCR1B &= ~(1 << CS12); // p.143 clkI/O
	TCCR1B |= (1 << CS11); // idem above
	TCCR1B |= (1 << CS10); // idem above
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for TIMER1_OVF
__attribute__((signal, used))
void	TIMER0_OVF_vect()
{
	static uint16_t	duty = 0;		// Current duty cycle (0 to OCR1A)
    static uint8_t	increasing = 1; // Flag (1: increasing, 0: decreasing)
	static uint16_t	counter = 0;
	const uint16_t	update_interval = 10;
	const uint16_t	increment = 2;

	counter++;
	if (counter >= update_interval)
	{
		counter = 0;

		if (increasing)
		{
			if (duty < ICR1)
				duty += increment;
			else
			{
				increasing = 0;
				duty -= increment;
			}
		}
		else
		{
			if (duty > 0)
				duty -= increment;
			else
			{
				increasing = 1;
				duty += increment;
			}
		}

		// Ensure duty cycle stays within bounds
		if (duty > ICR1)
			duty = ICR1;

		OCR1A = duty; // Update the duty cycle
	}
}

int	main(void)
{
	led_d2_init();
	timer0_interrupt_init();
	timer1_pwm_init();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Status Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}



// PWM Frequency Table for Timer0/Timer2 (8-bit Fast PWM, TOP=0xFF)
// Equation: F_PWM = F_CPU / (prescaler * 100)
// F_CPU = 16 MHz for ATmega328P at 16 MHz clock

// Prescaler | CS Bits   | Frequency (Hz) | Notes
// ----------|-----------|----------------|-------
//     1     |   0b001   |    160,000     | > 2 kHz => Ideal
//     8     |   0b010   |     20,000     | > 2 kHz => Ideal
//    64     |   0b011   |      2,500     | > 2 kHz => Ideal
//   256     |   0b100   |        625     | ok > 100 Hz (limit for flickering visible)
//  1024     |   0b101   |        156     | ok > 100 Hz (limit for flickering visible)

// Source : https://www.ti.com/document-viewer/lit/html/SSZTAY6
