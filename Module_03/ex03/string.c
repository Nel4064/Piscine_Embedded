#include <avr/io.h>

uint8_t	ft_strcpy(char *dest, const char *src)
{
	uint8_t i = 0;

	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}