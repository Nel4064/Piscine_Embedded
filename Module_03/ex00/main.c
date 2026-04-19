#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

void	led_init(void)
{
	// Port for LED_5 (PD3/LED_B, PD5/LED_R, PD6/lED_G) set as OUTPUT
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
}

void led_blinking_sequence(void)
{
	uint8_t patterns[] = 
	{
		0b00100000,  // LED5 (PD5/LED_R)
		0b01000000,  // LED5 (PD6/LED_G)
		0b00001000,  // LED5 (PD3/LED_B)
	};

	for (uint8_t i = 0; i < 3; i++)
	{
		PORTD = patterns[i];
		_delay_ms(1000);
	}
}

int main(void)
{
	led_init();
	while (1)
		led_blinking_sequence();
}