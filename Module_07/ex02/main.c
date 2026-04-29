#include <avr/io.h>
#include "uart.h"
#include "delay_us_and_ms.h"
#include "eeprom_rw.h"
#include "node_cmd.h"
#include "parsing.h"
#include "node.h"

#ifndef F_CPU
 #define F_CPU		16000000UL
#endif

const uint16_t g_slot_addresses[NUM_SLOTS] =
{
	0x0000, // Slot 0: 0x0000 to 0x002A (43 bytes)
	0x002B, // Slot 1: 0x002B to 0x0055 (43 bytes)
	0x0056, // Slot 2: 0x0056 to 0x0080 (43 bytes)
	0x0081  // Slot 3: 0x0081 to 0x00AB (43 bytes)
};

uint16_t	g_slot_address_max = g_slot_addresses[NUM_SLOTS - 1] + NODE_SIZE;
t_node		g_current_node = {0};
uint8_t		g_current_slot = 0;

uint8_t find_valid_node(t_node *node)
{
	for (uint8_t i = 0; i < NUM_SLOTS; i++)
	{
		t_node current_node;
		eeprom_read_block(g_slot_addresses[i], (uint8_t *)&current_node, NODE_SIZE);

		if (is_node_valid(&current_node) == NODE_ERROR_NONE)
		{
			*node = current_node;
			g_current_slot = i;
			return (EEPROM_ERROR_NONE);
		}
	}
	return (EEPROM_NO_VALID_SLOT);
}

int main(void)
{
	char		user_str[MAX_USER_STR_LENGTH];
	char		command[MAX_USER_STR_LENGTH];
	char		param[MAX_USER_STR_LENGTH];
	uint8_t		parsing_err;
	uint8_t		eeprom_err;

	uart_init();
	// init_timer_for_delay();

	if (find_valid_node(&g_current_node) != EEPROM_ERROR_NONE)
		uart_printstr("No valid node.\r\n");
	else
	{
		uart_printstr("Node configured at address : 0x");
		uart_tx_hex_uint16(g_slot_addresses[g_current_slot]);
		uart_printstr(" / Slot : ");
		uart_tx_dec_int16(g_current_slot);
		uart_printstr("\r\n");
	}

	while (1)
	{
		ft_memset(user_str, 0, MAX_USER_STR_LENGTH);
		ft_memset(command, 0, MAX_USER_STR_LENGTH);
		ft_memset(param, 0, MAX_USER_STR_LENGTH);

		uart_printstr("> ");
		parsing_err = get_and_test_str(user_str, MAX_USER_STR_LENGTH, command, param);

		if (parsing_err != PARSING_ERROR_NONE)
			continue ;

		if (ft_strcmp(command, "STATUS") == 0)
			node_status(param);
		else if (ft_strcmp(command, "SET_ID") == 0)
			node_set_id(param);
		else if (ft_strcmp(command, "SET_PRIO") == 0)
			node_set_prio(param);
		else if (ft_strcmp(command, "SET_TAG") == 0)
			node_set_tag(param);
		else if (ft_strcmp(command, "FACTORY_RESET") == 0)
			node_factory_reset(param);
		else if (ft_strcmp(command, "HEXDUMP") == 0)
			eeprom_hexdump_slots();
		else
			uart_printstr("Error: Unknown command.\r\n");
	}
}
