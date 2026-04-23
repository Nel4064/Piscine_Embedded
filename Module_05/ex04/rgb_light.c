#include <avr/io.h>

void	init_wheel_led(void)
{
	// Port for LED_5 (PD3/LED_B, PD5/LED_R, PD6/lED_G) set as OUTPUT
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
}

void	init_rgb(void)
{
	// Enable Timer/Counter0
	// PRR &= ~(1 << PRTIM0);  // Not strictly necessary

	// Set Clock Select to 0b010 = 8 (Vs. 1/8/64/256) => 7kHtz, cf. table below.
	TCCR0B &= ~(1 << CS02); // p. 117 clkI/O/(No prescaling)
	TCCR0B |= (1 << CS01);
	TCCR0B &= ~(1 << CS00);

	// Set mode of operation to Fast PWM, TOP = 0xFF (WGM02:00 = 0b011)
	// with PWM = Pulse Width Modulation
	TCCR0B &= ~(1 << WGM02); 
	TCCR0A |= (1 << WGM01);
	TCCR0A |= (1 << WGM00);

	// Set mode of operation to non inverting mode
	// => p.113 Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM.
	TCCR0A |= (1 << COM0A1); // p.113 - Table 15-3. Compare Output Mode, Fast PWM Mode
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0B1); // p.114 - Table 15-6. Compare Output Mode, Fast PWM Mode
	TCCR0A &= ~(1 << COM0B0);

	// Enable Timer/Counter2
	// PRR &= ~(1 << PRTIM2);  // Not strictly necessary

	// Set Clock Select to 0b010 = 8 (Vs. 1/8/64/256) => 7kHtz, cf. table below.
	TCCR2B &= ~(1 << CS22); // p. 165 clkI/O/(No prescaling)
	TCCR2B |= (1 << CS21);
	TCCR2B &= ~(1 << CS20);

	// Set mode of operation to Fast PWM, TOP = 0xFF (WGM22:20 = 0b011)
	// with PWM = Pulse Width Modulation
	TCCR2B &= ~(1 << WGM22); 
	TCCR2A |= (1 << WGM21);
	TCCR2A |= (1 << WGM20);

	// Set mode of operation to non inverting mode
	// => p.113 Clear OC2B on Compare Match, set OC2B at BOTTOM.
	TCCR2A |= (1 << COM2B1); // p.163 - Table 18-6. Compare Output Mode, Fast PWM Mode
	TCCR2A &= ~(1 << COM2B0);

	// Re-start clock at 0 after all the setting above (not stricly necessary... but why not!)
	// Pseudo synchronisation at 1 cylce approximation
	TCNT0 = 0; // p.117 => TCNT0 = 0b00000000;
	TCNT2 = 0; // TCNT2 = 0b00000000;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r; // Red on OC0B (PD5)
	OCR0A = g; // Green on OC0A (PD6)
	OCR2B = b; // Blue on OC2B (PD3)
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
		set_rgb(255 - pos * 3, 0, pos * 3);
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	}
	else
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

uint8_t hex_char_to_val(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return (0);
}