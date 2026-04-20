#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define SW1_PRESSED	!(PIND & (1 << PIND2))

volatile uint8_t debounce_timer = 0;  // Global variable to track debounce state

void	led_d1_init(void)
{
	DDRB |= (1 << DDB0); // p.101 Set PB0/PCINT0 (= LED_D1) as OUTPUT
}

void	switch_sw1_init(void)
{
	DDRD &= ~(1 << DDD2); // p.101 Set PD2/INT0 (= SW1) as INPUT
	PORTD |= (1 << PORTD2); // p.101 Enable internal pull-up resistor (Rpu) for PD2 (= SW1)
}

void	switch_sw1_interupt_init(void)
{
	EIMSK |= (1 << INT0); // p.81 When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the external pin interrupt is enabled. Activity on the pin will cause an interrupt request even if INT0 is configured as an output
	EICRA |= (1 << ISC01); // p.80 The falling edge of INT0 generates an interrupt request.
	EICRA &= ~(1 << ISC00); // idem.
}

void	timer1_interrupt_init(void)
{
	TCCR1B &= ~(1 << CS12); // p.143 Timer/Counter stopped. No clock source (Timer/Counter stopped).
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B &= ~(1 << CS10); // idem above
	TIMSK1 |= (1 << TOIE1); // p.145 Timer/Counter1, Overflow Interrupt Enable
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for INT0
__attribute__((signal, used))
void	INT0_vect()
{
	if (debounce_timer == 0)
	{
		debounce_timer = 1; // Set debounce timer flag
		TCNT1 = 65536 - 312; // ~20 ms debounce, i.e. 312 * (1 / (F_CPU / 1024)) = 19.968 ms
		TCCR1B |= (1 << CS12); // p.143 Timer/Counter start with clkI/O/1024
		TCCR1B &= ~(1 << CS11); // idem above
		TCCR1B |= (1 << CS10); // idem above
	}
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for TIMER1_OVF
__attribute__((signal, used))
void	TIMER1_OVF_vect()
{
	if (SW1_PRESSED)
		PORTB ^= (1 << PORTB0); // Togle LED on/off / PB0 = 1/0
	TCCR1B &= ~(1 << CS12); // p.143 Timer/Counter stopped. No clock source (Timer/Counter stopped).
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B &= ~(1 << CS10); // idem above
	debounce_timer = 0; // Reset debounce timer flag
}

int	main(void)
{
	led_d1_init();
	timer1_interrupt_init();
	switch_sw1_init();
	switch_sw1_interupt_init();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Status Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}

// find /usr -name "io.h" | grep avr
// cat /usr/avr/include/avr/io.h
// cat /usr/avr/include/avr/iom328p.h
// find /usr -name "interrupt.h" | grep avr