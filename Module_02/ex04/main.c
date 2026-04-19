#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

#define BAUD_RATE	115200
#define PRIVATE		1
#define PUBLIC		0
#define MAX_LENGTH	50
#define TRUE  		1
#define FALSE 		0

// cf. Example / DS40002061B-page 185
void uart_init()
{
	// Define UBRR0 register value
	uint16_t ubrr;
	ubrr = 16; // (cf. DS40002061B-p.199, table 20-7, 16MHz/115.2k/U2X0 = 1)

	// Set baud rate (DS40002061B-page 204)
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	// Enable transmitter and receiver (DS40002061B-page 201)
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);

	// Set USART mode to Asynchronous USART	(DS40002061B-p.202)
	UCSR0C &= ~(1 << UMSEL01);
	UCSR0C &= ~(1 << UMSEL00);

	// Set frame format (DS40002061B-p.202/203)
	// no parity
	UCSR0C &= ~(1 << UPM01);
	UCSR0C &= ~(1 << UPM00);
	// 1-stop bit
	UCSR0C &= ~(1 << USBS0);
	// 8-bit data
	UCSR0C &= ~(1 << UCSZ02);
	UCSR0C |= (1 << UCSZ01);
	UCSR0C |= (1 << UCSZ00);

	// Set Double USART Transmission Speed (DS40002061B-p.199, Table 20-7, 2.1% error vs. 3.5%)
	// i.e. Sample/Rx clock frequency = 1/8 * Tx clock frequency
	UCSR0A |= (1 << U2X0); // DS40002061B-p.200/201 U2Xn bit and register and p.181 - Figure 20-2.
}

// cf. Example / DS40002061B-page 186
void uart_tx(unsigned char c)
{
	/// Wait for empty transmit buffer, , i.e UDRE0 flag = 1
	// DS40002061B-p.186 The function simply waits for the transmit buffer to be empty by checking the UDREn Flag, before loading it with
	// new data to be transmitted.
	while ((UCSR0A & (1<<UDRE0)) == 0)
	{}

	// Put data into buffer, sends the data
	UDR0 = c;
}

// cf. Example / DS40002061B-page 189
char uart_rx(void)
{
	// Wait for data to be received, i.e RXC0 flag = 1
	while ((UCSR0A & (1<<RXC0)) == 0)
	{}

	// Get and return received data from buffer
	return (UDR0);
}

void uart_printstr(const char* str)
{
	while (*str)
		uart_tx(*str++);
}

void get_and_echostr(char* str, uint8_t length, uint8_t status)
{
	char c;
	uint8_t i = 0;
	uint8_t cursor = 0;

	while (i < length - 1)
	{
		c = uart_rx();
		if (c == 127) // if = DEL (backspace)
		{
			if (cursor > 0)
			{
				uart_printstr("\b \b");
				i--;
				cursor--;
			}
			continue;
		}
		if (c == '\r')
			break;
		str[i] = c;
		if (status == PUBLIC)
			uart_tx(c);
		else if (status == PRIVATE)
			uart_tx('*');
		i++;
		cursor++;
	}
	str[i] = '\0';

	if (i == length - 1)
		uart_printstr("\r\n    Max length reached! Invalid input.");

	// Consume extra characters if buffer is full
	if (i == length - 1 && c != '\r')
	{
		while (1)
		{
			c = uart_rx();
			if (c == '\r')
				break;
		}
	}
}

void	led_init(void)
{
	// LEDs (PB0/D1, PB1/D2, PB2/D3, PB4/D4) = OUTPUT
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
}

void led_blinking_sequence(void)
{
	uint8_t patterns[] = 
	{
		0b00000001,  // LED1 (PB0)
		0b00000010,  // LED2 (PB1)
		0b00000100,  // LED3 (PB2)
		0b00010000,  // LED4 (PB4)
		0b00000100,  // LED3 (PB2)
		0b00000010,  // LED2 (PB1)
	};

	for (uint8_t cycle = 0; cycle < 5; cycle++)
	{
		for (uint8_t i = 0; i <= 5; i++)
		{
			PORTB = patterns[i];
			_delay_ms(200);
		}
	}
	PORTB = 0;
}

uint8_t	ft_strcmp(char *str1, char *str2)
{
	uint8_t i = 0;

	while (str1[i] != '\0' && str1[i] == str2[i])
		i++;
	if (str1[i] == str2[i])
		return (TRUE);
	else
		return (FALSE);
}

int main(void)
{
	char	usr_nm[MAX_LENGTH];
	char	pwd[MAX_LENGTH];
	char	*usr_nm_toto = "toto";
	char	*pwd_toto = "qwerty123";

	led_init();
	uart_init();

	while (1)
	{
		uart_printstr("Welcome! Please login.\r\n");
		uart_printstr("    Username :  ");
		get_and_echostr(usr_nm, MAX_LENGTH, PUBLIC);
		uart_printstr("\r\n");
		uart_printstr("    Password :  ");
		get_and_echostr(pwd, MAX_LENGTH, PRIVATE);
		uart_printstr("\r\n");
		if (ft_strcmp(usr_nm, usr_nm_toto)
				&& ft_strcmp(pwd, pwd_toto))
		{
			uart_printstr("Hello ");
			uart_printstr(usr_nm);
			uart_printstr("!\r\n");
			uart_printstr("Login success! Let's do nothing!\r\n");
			led_blinking_sequence();
			// break ;
		}
		else
			uart_printstr("Login failed! Try again!\r\n");
		uart_printstr("\r\n");
		uart_printstr("+++++++++++++++++++++++++++++++++++++++++\r\n");
		uart_printstr("\r\n");
	}
}