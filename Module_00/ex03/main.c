#include <avr/io.h>
#include <util/delay.h>

int main()
{
	DDRB |= (1 << DDB0); // PB0/D1 = OUTPUT
	DDRD &= ~(1 << DDD2); // PD2/SW1 = INPUT
	PORTD |= (1 << PORTD2); // PD2/SW1 = Enable internal pull-up resistor (Rpu)

	uint8_t was_button_pressed = 0; // 0 = not pressed, 1 = pressed

	while (1)
	{
		uint8_t is_button_pressed = !(PIND & (1 << PIND2)); // digital read

		if (!was_button_pressed && is_button_pressed)
		{
			_delay_ms(20);
			is_button_pressed = !(PIND & (1 << PIND2)); // re-digital read
			if (is_button_pressed)
				PORTB ^= (1 << PORTB0);
		}
		was_button_pressed = is_button_pressed;
	}

	return (0);
}

// How To Implement Switch Debounce | Switch Bounce Explained
// https://www.picotech.com/library/articles/blog/what-is-switch-bounce-how-to-implement-debounce
// => Debounce = wait and recheck after a few ms