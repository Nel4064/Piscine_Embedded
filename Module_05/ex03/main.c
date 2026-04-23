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

void init_adc(void)
{
	// Set internal 1.1V voltage reference
	ADMUX |= (1 << REFS1); // DS40002061B-page 257 and 256 The internal 1.1V voltage reference must also be selected for the ADC voltage reference source in the temperature sensor measurement
	ADMUX |= (1 << REFS0);

	// Select right-adjust result for 10-bit resolution (DS40002061B-page 259)
	ADMUX &= ~(1 << ADLAR); // DS40002061B-page 257 Write one to ADLAR to left adjust the result. Otherwise, the result is right adjusted

	// Select ADC8 (on-chip temperature sensor)
	ADMUX |= (1 << MUX3); // DS40002061B-page 256 (MUX3..0 = 0b1000 => ADC8)
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX0);

	// Enable ADC (i.e. Enable analogic to digital convesrion)
	ADCSRA |= (1 << ADEN); // DS40002061B-page 258 Writing this bit to one enables the ADC. By writing it to zero, the ADC is turned off

	// Set Prescaler 128 (125kHz ADC clock)
	// DS40002061B-page 249 => By default, the successive approximation circuitry requires an input clock frequency between 50kHz and 200kHz to get maximum resolution.
	ADCSRA |= (1 << ADPS2); // DS40002061B-page 259
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0);

	// Enable auto-triggering
	// => When this bit is written to one, Auto Triggering of the ADC is enabled. The ADC will start a conversion on a positive edge of the selected trigger signal
	ADCSRA |= (1 << ADATE); // DS40002061B-page 258

	// Set trigger on Timer1 Compare Match B (ADTS2..0 = 0b101)
	// => Conversion will be triggered by the rising edge of the selected Interrupt Flag.
	ADCSRB |= (1 << ADTS2); // DS40002061B-page 260
	ADCSRB &= ~(1 << ADTS1);
	ADCSRB |= (1 << ADTS0);

	// Enable ADC interrupt
	// => When this bit is written to one and the I-bit in SREG is set, the ADC Conversion Complete Interrupt is activated.
	ADCSRA |= (1 << ADIE); // DS40002061B-page 259
}

void init_timer1(void)
{
	// Select CTC mode (Clear Timer on Compare match)
	TCCR1B &= ~(1 << WGM13); // DS40002061B-page 141
	TCCR1B |= (1 << WGM12);
	TCCR1A &= ~(1 << WGM11);
	TCCR1A &= ~(1 << WGM10);

	// Select Prescaler 64
	TCCR1B &= ~(1 << CS12); // DS40002061B-page 143
	TCCR1B |= (1 << CS11);
	TCCR1B |= (1 << CS10);

	// Set TOP value for 20ms period (= 50Hz = 1 / 20ms)
	// F_CTC = F_CPU / (prescaler * (1 + TOP))
	// TOP = F_CPU / (F_CTC * prescaler) - 1 = 16,000,000 / (64 * 50) - 1 = 4999 
	OCR1A = 4999;
	// Note : // Timer 1 is selected over Timer 0/2 because 16,000,000 / (50 * 1204) - 1 = 312 > 255

	// Trigger ADC at 20ms = Trigger flag OC1B at the same time/frequence as OC1A 
	OCR1B = 4999;

	// Disable Timer1 Compare Match B interrupt
	TIMSK1 &= ~(1 << OCIE1B);
}

// DS40002061B-p.74 Interrupt Vectors = ISR name
__attribute__((signal, used))
void	ADC_vect()
{
	uint16_t	k = 1; // roughly k = 1 (1 LSB/°C at 10-bit) 
	// Slope of linear regression = k : From 85°C - (-45°C) = 130°C and 380mV - 242mV = 138mV => 1.061 mV/°C
	// k = (380 - 242) / (85 - (-45))
	// k = 138 / 130
	// k = 1.0615 mV/°C
	// 1 LSB = 1100mV / 1024 = 1.074 mV/LSB
	// => k = 1.061 mV/°C ÷ 1.074 mV/LSB = 0.988 LSB/°C
	uint16_t	t_os = 290;
	// Y-Intercept/constant of linear regression
	// V = k * temperature + t_os
	// 242 = 1.0615 * (-45) + t_os
	// 242 = -47.77 + tos
	// t_os = 289.77 mV

	uint16_t	temperature = ((ADCL | (ADCH << 8)) - t_os) / k; // DS40002061B-page 257 formula

	TIFR1 |= (1 << OCF1B); // DS40002061B-page 145 Clear OCF1B flag
	// => OCF1B is "not" automatically cleared when the Output Compare Match B Interrupt Vector is "not" executed. Alternatively, OCF1B can be cleared by writing a logic one to its bit location.

	uart_tx_uint10(temperature);
	// uart_tx("0123456789abcdef"[(temperature & 0x0300) >> 8]);
	// uart_tx("0123456789abcdef"[(temperature & 0x00F0) >> 4]);
	// uart_tx("0123456789abcdef"[(temperature & 0x000F) >> 0]);
	uart_tx('\r');
	uart_tx('\n');

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
	init_adc();
	init_timer1();
	init_uart();
	SREG |= (1 << 7); // DS40002061B-p.20 / Global Interrupt Enable on SREG Status Register / = sei() from <avr/interrupt.h>
	while (1)
	{}
}
