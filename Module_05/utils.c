#include <avr/io.h>

uint8_t hex_char_to_val(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return (0);
}

char val_to_hex_char(uint8_t val)
{
	const char hex_chars[] = "0123456789abcdef";

	return (hex_chars[val & 0x0F]);
}