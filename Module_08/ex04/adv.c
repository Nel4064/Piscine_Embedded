#include "adv.h"

void init_adc(void)
{
	// Set AVCC as reference (= 5V)
	ADMUX &= ~(1 << REFS1); // DS40002061B-page 257.
	ADMUX |= (1 << REFS0);

	// Select left-adjust result for "8-bit like" resolution (DS40002061B-page 259)
	// => If the result is left adjusted and no more than 8-bit precision is required, it is sufficient to read ADCH
	ADMUX |= (1 << ADLAR); // DS40002061B-page 257 Write one to ADLAR to left adjust the result. Otherwise, the result is right adjusted
	
	// Select ADC0 (potentiometer) = Pin for RV1/ADC_POT/PC0/ADC0
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
