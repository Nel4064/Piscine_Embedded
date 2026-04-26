#include "delay_us_and_ms.h"

void init_timer1_normal_mode(void)
{
	// Set Normal mode for Timer/Counter Mode of Operation (WGM12:WGM10 = 0b0000)
	TCCR1B &= (1 << WGM12); // p. 141 Timer/Counter Mode of Operation
	TCCR1A &= (1 << WGM11); // idem above
	TCCR1A &= (1 << WGM10); // idem above
	// TOP = 0xFF

	// Set Normal port operation, OC1A/OC1B disconnected (no use / no LED, etc.)
	TCCR1A &= ~(1 << COM1A1); // p.140 Normal port operation, OC1A/OC1B disconnected
	TCCR1A &= ~(1 << COM1A0); // idem above

	// Set Clock Select to 0b011 = 8 (Vs. 1/8/64/256/1028)
	// => Prescaler = 8, 2 µs per tick at 16 MHz
	TCCR1B &= ~(1 << CS12); // p.143 clkI/O
	TCCR1B |= (1 << CS11); // idem above
	TCCR1B &= ~(1 << CS10); // idem above
	// F_Overflow = F_CPU / (prescaler * (1 + TOP)) = 16,000,000 / (8 * 65536) = 30.517 Hz
}

void init_timer_for_delay(void)
{
	init_timer1_normal_mode();
}

void delay_us(uint16_t us)
{
	TCNT1 = 0;
	while (TCNT1 < us * 0.5)
	{}
}

void delay_ms(uint16_t ms)
{
    while (ms--)
        delay_us(1000);
}