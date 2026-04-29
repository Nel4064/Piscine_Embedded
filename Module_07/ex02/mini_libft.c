#include "mini_libft.h"

uint8_t ft_memcmp(const void *s1, const void *s2, uint8_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	for (uint8_t i = 0; i < n; i++)
	{
		if (p1[i] != p2[i])
			return (p1[i] - p2[i]);
	}
	return (0);
}

void *ft_memcpy(void *dest, const void *src, uint8_t n)
{
	uint8_t *d = (uint8_t *)dest;
	const uint8_t *s = (const uint8_t *)src;

	while (n--)
		*d++ = *s++;

	return (dest);
}

void *ft_memset(void *s, uint8_t c, uint8_t n)
{
	uint8_t *ptr = (uint8_t *)s;
	while (n-- > 0)
		*ptr++ = (uint8_t)c;
	return s;
}

void ft_toupper(char *str)
{
	while (*str)
	{
		if (*str >= 'a' && *str <= 'z')
			*str -= 'a' - 'A';
		str++;
	}
}

uint8_t ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	return (*s1 - *s2);
}

uint8_t ft_strlen(char *str)
{
	uint8_t count = 0;

	while (*str)
	{
		count++;
		str++;
	}

	return (count);
}

uint8_t is_empty_string(const char *str)
{
	return (str == NULL || *str == '\0');
}

uint8_t is_valid_uint32(const char *str, uint32_t *value)
{
	uint32_t result = 0;
	uint8_t i = 0;

	if (is_empty_string(str))
		return (FALSE);

	while (str[i] != '\0')
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (result > (UINT32_MAX - (str[i] - '0')) / 10)
				return (FALSE); // Overflow
			result = result * 10 + (str[i] - '0');
		}
		else
			return (FALSE); // Invalid character
		i++;
	}

	*value = result;
	return (TRUE);
}

uint8_t is_valid_int16(const char *str, int16_t *value)
{
	int32_t result = 0;
	uint8_t i = 0;
	uint8_t is_negative = 0;

	if (is_empty_string(str))
		return (FALSE);

	if (str[i] == '-')
	{
		is_negative = 1;
		i++;
	}

	if (str[i] == '\0')
		return (FALSE);

	while (str[i] != '\0')
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (is_negative)
			{
				if (result < (INT16_MIN + (str[i] - '0')) / 10)
					return (FALSE); // Overflow
			}
			else 
			{
				if (result > (INT16_MAX - (str[i] - '0')) / 10)
					return (FALSE); // Overflow
			}
			result = result * 10 + (str[i] - '0');
		}
		else
			return (FALSE); // Invalid character
		i++;
	}

	if (is_negative)
		result = -result;

	if (result < INT16_MIN || result > INT16_MAX)
		return (FALSE);

	*value = (int16_t)result;
	return (TRUE);
}