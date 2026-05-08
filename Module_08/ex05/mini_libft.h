#ifndef MINI_LIBFT_H
# define MINI_LIBFT_H

#include <avr/io.h>

#ifndef TRUE
# define TRUE	((uint8_t)1)
#endif

#ifndef FALSE
# define FALSE	((uint8_t)0)
#endif

#ifndef NULL
# define NULL	((void *)0)
#endif

// !WARNING!
// Most functions are assuming only uint8_t_MAX = 255 characters length!

uint8_t ft_memcmp(const void *s1, const void *s2, uint8_t n);
void	*ft_memcpy(void *dest, const void *src, uint8_t n);
void	*ft_memset(void *s, uint8_t c, uint8_t n);
void	ft_toupper(char *str);
uint8_t ft_strcmp(const char *s1, const char *s2);
uint8_t ft_strlen(char *str);
uint8_t is_empty_string(const char *str);
uint8_t is_valid_uint32(const char *str, uint32_t *value);
uint8_t is_valid_int16(const char *str, int16_t *value);

#endif