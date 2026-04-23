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
	// Set AVCC as reference (= 5V)
	ADMUX &= ~(1 << REFS1); // DS40002061B-page 257.
	ADMUX |= (1 << REFS0);

	/// Select left-adjust result for "8-bit like" resolution (DS40002061B-page 259)
	// => If the result is left adjusted and no more than 8-bit precision is required, it is sufficient to read ADCH
	ADMUX |= (1 << ADLAR); // DS40002061B-page 257 Write one to ADLAR to left adjust the result. Otherwise, the result is right adjusted
	
	// Select ADC0 (potentiometer) = Pin for RV1/ADC_POT/PC0/ADC0
	// Select ADC1 (light dependant resistor) = Pin for R14/ADC_LDR/PC1/ADC1
	// Select ADC2 (thermistor) = Pin for R20/ADC_NTC/PC2/ADC2
	// Set starting measure with ADC0 / potentiometer
	ADMUX &= ~(1 << MUX3); // DS40002061B-page 257 (MUX3..0 = 0b0000 => ADC0)
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

	// Set TOP value for 20ms/3 (6.666ms) period (=150 Hz, i.e. 1/(20ms/3))
	// F_CTC = F_CPU / (prescaler * (1 + TOP))
	// TOP = F_CPU / (F_CTC * prescaler) - 1 = 16,000,000 / (64 * 150) - 1 = 1666
	OCR1A = 1666;

	// Trigger ADC at 6.666ms = Trigger flag OC1B at the same time/frequence as OC1A 
	OCR1B = 1666;

	// Disable Timer1 Compare Match B interrupt
	TIMSK1 &= ~(1 << OCIE1B);
}

// DS40002061B-p.74 Interrupt Vectors = ISR name
__attribute__((signal, used))
void	ADC_vect()
{
	static volatile uint8_t	count = 0;
	static volatile uint8_t	data_set[3];

	TIFR1 |= (1 << OCF1B); // DS40002061B-page 145 Clear OCF1B flag
	// => OCF1B is "not" automatically cleared when the Output Compare Match B Interrupt Vector is "not" executed.
	// Alternatively, OCF1B can be cleared by writing a logic one to its bit location.

	if (count < 3)
	{
		// Store the current ADC reading
		data_set[count] = ADCH;

		// Select the next ADC channel
		if (count == 0)
		{
			ADMUX &= ~(1 << MUX3); // DS40002061B-page 257 (MUX3..0 = 0b0001 => ADC1)
			ADMUX &= ~(1 << MUX2);
			ADMUX &= ~(1 << MUX1);
			ADMUX |= (1 << MUX0);
			ADCSRA &= ~(1 << ADIE); // Disable ADC interrupt
			ADCSRA |= (1 << ADSC); // Start a dummy conversion
			while (ADCSRA & (1 << ADSC)); // Wait for dummy conversion to complete
			// => The dummy conversion ensures the ADC stabilizes on the new channel before the next auto-triggered conversion.
			// DS40002061B-page 251 "If Auto Triggering is used, the exact time of the triggering event can be indeterministic.
			// Special care must be taken when updating the ADMUX Register, in order to control which conversion
			// will be affected by the new settings.""
			
			ADCSRA |= (1 << ADIE); // Re-enable ADC interrupt
		}
		else if (count == 1)
		{
			ADMUX &= ~(1 << MUX3); // DS40002061B-page 257 (MUX3..0 = 0b0010 => ADC2)
			ADMUX &= ~(1 << MUX2);
			ADMUX |= (1 << MUX1);
			ADMUX &= ~(1 << MUX0);
			ADCSRA &= ~(1 << ADIE); // Disable ADC interrupt
			ADCSRA |= (1 << ADSC); // Start a dummy conversion
			while (ADCSRA & (1 << ADSC)); // Wait for dummy conversion to complete
			ADCSRA |= (1 << ADIE); // Re-enable ADC interrupt
		}
		else if (count == 2)
		{
			ADMUX &= ~(1 << MUX3); // DS40002061B-page 257 (MUX3..0 = 0b0000 => ADC0)
			ADMUX &= ~(1 << MUX2);
			ADMUX &= ~(1 << MUX1);
			ADMUX &= ~(1 << MUX0);
			ADCSRA &= ~(1 << ADIE); // Disable ADC interrupt
			ADCSRA |= (1 << ADSC); // Start a dummy conversion
			while (ADCSRA & (1 << ADSC)); // Wait for dummy conversion to complete
			ADCSRA |= (1 << ADIE); // Re-enable ADC interrupt
		}
	}
	if (count == 2)
	{
		for (uint8_t i = 0; i < 3; i++)
		{
			uart_tx("0123456789abcdef"[(data_set[i] & 0xF0) >> 4]);
			uart_tx("0123456789abcdef"[(data_set[i] & 0x0F) >> 0]);
			if (i != 2)
			{
				uart_tx(',');
				uart_tx(' ');
			}		
		}
		uart_tx('\r');
		uart_tx('\n');
		count = 0;
	}
	else
		count++;

	// // Activity check for DEBUG
	// static uint8_t	activity_count = 0;
	// activity_count++;
	// if (activity_count >= 150)
	// {
	// 	PORTB ^= (1 << PORTB0);
	// 	activity_count = 0;
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
