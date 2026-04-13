#include <avr/io.h>
#include <util/delay.h>

int main()
{
	// Configure PB0 (LED D1) as output
	DDRB |= (1 << DDB0);

	// Configure PD2 (SW1) as input
	DDRD &= ~(1 << DDD2);

	// Enable internal pull-up resistor on PD2
	PORTD |= (1 << PORTD2);

	// Variable to track the previous button state
	uint8_t prev_status = 1;  // Initially, button is not pressed (due to pull-up)

	while (1)
	{
		// Read the current button state
		uint8_t is_button_pressed = !(PIND & (1 << PIND2));

		// Check for a transition from released (1) to pressed (0)
		if (prev_status && !is_button_pressed)
		{
			// Toggle the LED state
			PORTB ^= (1 << PORTB0);
			//_delay_ms(5);  // Debounce delay
		}

		// Update the previous button state
		prev_status = is_button_pressed;
	}

	return 0;
}