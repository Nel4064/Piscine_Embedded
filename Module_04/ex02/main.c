#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define IS_SW1_PRESSED	!(PIND & (1 << PIND2))
#define IS_SW2_PRESSED	!(PIND & (1 << PIND4))

volatile uint8_t debounce_timer = 0;  // Global variable to track debounce state
volatile uint8_t was_sw1_pressed = 0;
volatile uint8_t was_sw2_pressed = 0;

void	led_init(void)
{
	// LEDs (PB0/D1, PB1/D2, PB2/D3, PB4/D4) = OUTPUT
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
}

void	switch_sw1_init(void)
{
	DDRD &= ~(1 << DDD2); // p.101 Set PD2/PCINT18 (= SW1) as INPUT
	PORTD |= (1 << PORTD2); // p.101 Enable internal pull-up resistor (Rpu) for PD2 (= SW1)
}

void	switch_sw1_interrupt_init(void)
{
	PCMSK2 |= (1 << PCINT18); // p.83 Each PCINT[23:16]-bit selects whether pin change interrupt is enabled on the corresponding I/O pin.
	PCICR |= (1 << PCIE2);    // p.82 Enable pin change interrupt 2 group
}

void	switch_sw2_init(void)
{
	DDRD &= ~(1 << DDD4); // p.101 Set PD4/PCINT20 (= SW2) as INPUT
	PORTD |= (1 << PORTD4); // p.101 Enable internal pull-up resistor (Rpu) for PD4 (= SW2)
}

void	switch_sw2_interrupt_init(void)
{
	PCMSK2 |= (1 << PCINT20); // p.83 Each PCINT[23:16]-bit selects whether pin change interrupt is enabled on the corresponding I/O pin.
	PCICR |= (1 << PCIE2);    // p.82 Enable pin change interrupt 2 group
}

void	timer1_interrupt_init(void)
{
	TCCR1B &= ~(1 << CS12); // p.143 Timer/Counter stopped. No clock source (Timer/Counter stopped).
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B &= ~(1 << CS10); // idem above
	TIMSK1 |= (1 << TOIE1); // p.145 Timer/Counter1, Overflow Interrupt Enable
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for INT0
// p.82 he corresponding interrupt of Pin Change Interrupt Request is executed from the PCI2 Interrupt Vector. PCINT[23:16] pins are enabled individually by the PCMSK2 Register.
__attribute__((signal, used))
void	PCINT2_vect()
{
	uint8_t	is_sw1_pressed = IS_SW1_PRESSED;
	uint8_t	is_sw2_pressed = IS_SW2_PRESSED;

	if ((!was_sw1_pressed && is_sw1_pressed) || (!was_sw2_pressed && is_sw2_pressed))
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
	was_sw1_pressed = is_sw1_pressed;
	was_sw2_pressed = is_sw2_pressed;
}

// DS40002061B-p.74 Interrupt Vectors = ISR name for TIMER1_OVF
__attribute__((signal, used))
void	TIMER1_OVF_vect()
{
	static uint16_t	count = 0;
	uint8_t	is_sw1_pressed = IS_SW1_PRESSED;
	uint8_t	is_sw2_pressed = IS_SW2_PRESSED;

	if (is_sw1_pressed)
	{
		count++;
		was_sw1_pressed = is_sw1_pressed;
	}
	if (is_sw2_pressed)
	{
		count--;
		was_sw2_pressed = is_sw2_pressed;
	}
	PORTB = ((count & 0b00000111) | ((count & 0b00001000) << 1)); // Display count value
	TCCR1B &= ~(1 << CS12); // p.143 Timer/Counter stopped. No clock source (Timer/Counter stopped).
	TCCR1B &= ~(1 << CS11); // idem above
	TCCR1B &= ~(1 << CS10); // idem above
	debounce_timer = 0; // Reset debounce timer flag
}

int	main(void)
{
	led_init();
	switch_sw1_init();
	switch_sw1_interrupt_init();
	switch_sw2_init();
	switch_sw2_interrupt_init();
	timer1_interrupt_init();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Status Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}
