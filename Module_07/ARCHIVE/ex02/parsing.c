#include "parsing.h"

void ft_simple_strtok(char *str, char *command, char *param, uint8_t max_len)
{
	uint8_t i = 0;
	uint8_t space_pos = 0;
	bool found_space = FALSE;

	command[0] = '\0';
	param[0] = '\0';

	// Find the first space
	while (str[space_pos] != '\0' && space_pos < max_len)
	{
		if (str[space_pos] == ' ')
		{
			found_space = TRUE;
			break ;
		}
		space_pos++;
	}

	for (i = 0; i < space_pos && i < max_len - 1; i++)
		command[i] = str[i];
	command[i] = '\0';

	if (found_space)
	{
		uint8_t param_idx = 0;
		space_pos++;
		while (str[space_pos] != '\0' && param_idx < max_len - 1)
		{
			param[param_idx] = str[space_pos];
			space_pos++;
			param_idx++;
		}
		param[param_idx] = '\0';
	}
}

uint8_t get_and_test_str(char *user_str, uint8_t max_len, char *command, char *param)
{
	char c;
	uint8_t i = 0;

	// Read input until '\r' (Enter) or max_len is reached
	while (i < max_len - 1)
	{
		c = uart_rx();

		if (c == 27) // ESC
		{
			uart_rx();
			uart_rx();
			continue;
		}

		if (c < 32 && c != 127 && c != '\r')
			continue;

		if (c == 127) // DEL
		{
			if (i > 0)
			{
				uart_printstr("\b \b"); // Erase the character
				i--;
			}
			continue;
		}

		if (c == '\r')
			break;

		// Store the character
		user_str[i] = c;
		uart_tx(c);
		i++;
	}
	user_str[i] = '\0';
	uart_printstr("\r\n");

	if (i == max_len - 1)
	{
		uart_printstr("Max length reached! Invalid input. Press Enter to retry.\r\n");
		// Consume extra characters
		while (uart_rx() != '\r');
		return(PARSING_TOO_MANY_CHAR);
	}

	// Split into command and param
	ft_simple_strtok(user_str, command, param, max_len);
	ft_toupper(command);

	return (PARSING_ERROR_NONE);
}