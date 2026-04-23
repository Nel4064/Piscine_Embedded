#include <avr/io.h>
#include <util/delay.h>
#include "uart.c"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#ifndef BAUD_RATE
 #define BAUD_RATE	115200
#endif

void	activity_led_init(void)
{
	DDRB |= (1 << DDB0); // p.101 Set PB0 (= LED_D1) as OUTPUT
}

void i2c_init(void)
{
}

void i2c_start(void)
{
}

void i2c_stop(void)
{
}

// DS40002061B-p.74 Interrupt Vectors = ISR name
__attribute__((signal, used))
void	XXXXXXX_vect()
{
	// // Activity check for DEBUG
	// static uint8_t	count = 0;
	// count++;
	// if (count >= 50)
	// {
	// 	PORTB ^= (1 << PORTB0);
	// 	count = 0;
	// }
}

int main(void)
{
	// activity_led_init();
	i2c_init();
	i2c_start();
	i2c_stop();
	// SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Status Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}
