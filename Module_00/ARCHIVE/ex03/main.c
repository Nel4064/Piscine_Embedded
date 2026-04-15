#include <avr/io.h>
#include <util/delay.h>

int main()
{	
	DDRB |= (1 << DDB0); // Set PB0 (LED D1) as OUTPUT

	DDRD &= ~(1 << DDD2); // Set PD2 (SW1) as INPUT

	PORTD |= (1 << PORTD2); // Enable internal pull-up resistor for PD2

	uint8_t prev_status = 1;  // pull-up state at start-up (not pressed)
	uint8_t toggle_no_bounce = 0;

	while (1)
	{
		uint8_t is_button_pressed = !(PIND & (1 << PIND2));

		// On button pressed
		if (prev_status && !is_button_pressed && !toggle_no_bounce)
		{
			PORTB ^= (1 << PORTB0); // Toggle the LED state
			toggle_no_bounce = 1;
		}

		// On button released
		if (prev_status = is_button_pressed)
			toggle_no_bounce = 0;

		prev_status = is_button_pressed;

		_delay_ms(20);
	}

	// // Whithout debouncing
	// while (1)
	// {
	// 	uint8_t is_button_pressed = !(PIND & (1 << PIND2));
	// 	if (prev_status && !is_button_pressed)
	// 		PORTB ^= (1 << PORTB0);
	// 	prev_status = is_button_pressed;
	// }

	return 0;
}