#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define LED_RED     0b00100000 // LED5 (PD5/LED_R)
#define LED_GREEN   0b01000000 // LED5 (PD6/LED_G)
#define LED_BLUE    0b00001000 // LED5 (PD3/LED_B)
#define LED_YELLOW	(LED_RED | LED_GREEN)				// 0b01100000
#define LED_CYAN	(LED_GREEN | LED_BLUE)				// 0b01001000
#define LED_MAGENTA	(LED_RED | LED_BLUE)				// 0b00101000
#define LED_WHITE	(LED_RED | LED_GREEN | LED_BLUE)	// 0b01101000

void	led_init(void)
{
	// Port for LED_5 (PD3/LED_B, PD5/LED_R, PD6/lED_G) set as OUTPUT
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
}

void led_blinking_sequence(void)
{
	uint8_t patterns[] = 
	{
		LED_RED,
		LED_GREEN,
		LED_BLUE,
		LED_YELLOW,
		LED_CYAN,
		LED_MAGENTA,
		LED_WHITE
	};

	for (uint8_t i = 0; i < 7; i++)
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