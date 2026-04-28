#ifndef PARSING_H
# define PARSING_H

#include <avr/io.h>
#include "node.h"
#include "uart.h"
#include "mini_libft.h"

uint8_t get_and_test_str(char *user_str, uint8_t max_len, char *command, char *param);

#endif