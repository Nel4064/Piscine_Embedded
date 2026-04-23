#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	// LEDs (PB0/D1, PB1/D2, PB2/D3, PB4/D4) = OUTPUT
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);

	// Buttons (PD2/SW1, PD4/SW2) = INPUT + ENABLED
	DDRD &= ~((1 << DDD2) | (1 << DDD4));
	PORTD |= (1 << PORTD2) | (1 << PORTD4);

	// 0 = not pressed, 1 = pressed
	uint8_t	was_button_SW1_pressed = 0;
	uint8_t	was_button_SW2_pressed = 0;
	uint8_t	count = 0;

	while (1)
	{
		uint8_t	is_button_SW1_pressed = !(PIND & (1 << PD2));
		uint8_t	is_button_SW2_pressed = !(PIND & (1 << PD4));

		if (!was_button_SW1_pressed && is_button_SW1_pressed)
		{
			_delay_ms(20);
			is_button_SW1_pressed = !(PIND & (1 << PIND2));
			if (is_button_SW1_pressed)
				count++;
		}

		if (!was_button_SW2_pressed && is_button_SW2_pressed)
		{
			_delay_ms(20);
			is_button_SW2_pressed = !(PIND & (1 << PIND4));
			if (is_button_SW2_pressed && count > 0)
				count--;
		}
		PORTB = ((count & 0b00000111) | ((count & 0b00001000) << 1));
		was_button_SW1_pressed = is_button_SW1_pressed;
		was_button_SW2_pressed = is_button_SW2_pressed;
	}

	return (0);
}