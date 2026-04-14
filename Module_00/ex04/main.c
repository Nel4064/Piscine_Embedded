#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	// Set LEDs as OUTPUT
	DDRB |= (1 << PB0); // PB0 = D1
	DDRB |= (1 << PB1); // PB1 = D2
	DDRB |= (1 << PB2); // PB2 = D3
	DDRB |= (1 << PB4); // PB3 = MOSI, but PB4 = D4  

	// Set PD2 and PD4 buttons as INPUT
	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD4);

	// Turn on pull-up resistor for PD2 and PD4 buttons
	PORTD |= (1 << PD2);
	PORTD |= (1 << PD4);

	int toggle_no_bounce = 0;
	uint8_t count = 0b00000000;

	while (1)
	{
		// Increment
		if (!(PIND & (1 << PD2)))
		{
			if (toggle_no_bounce == 0)
			{
				toggle_no_bounce = 1;
				if (count < 0b00010111)
					count++;
				if (count == 0b00001000)
					count = 0b00010000;
				PORTB = count;
			}
		}
		// Decrement
		else if (!(PIND & (1 << PD4)))
		{
			if (toggle_no_bounce == 0)
			{
				toggle_no_bounce = 1;
				if (count)
					count--;
				if (count == 0b00001111)
					count = 0b00000111;
				PORTB = count;
			}
		}
		else
			toggle_no_bounce = 0;

		_delay_ms(20);
	}

	return 0;
}