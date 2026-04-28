#include "node_cmd.h"

uint16_t	add_checksum_16(const uint8_t *data, uint16_t len)
{
	uint32_t sum = 0;
	for (uint16_t i = 0; i < len; i++)
		sum += data[i];
	return ((uint16_t)((sum & 0x0000FFFF) >> 0) + (uint16_t)((sum & 0xFFFF0000) >> 16));
}

uint8_t	is_node_valid(const t_node *node)
{
	if (node->magic_nb != NODE_MAGIC_NUMBER)
		return (MAGIC_NB_UNMATCH);
	uint16_t calculated_check = add_checksum_16((uint8_t *)&node, sizeof(t_node) - 2);
	return (calculated_check == node->integrity_check);
	
	if (calculated_check != node->integrity_check)
		return (INTEGRITY_CHECK_FAILED);

	return (NODE_ERROR_NONE);
}

uint8_t	is_tag_valid(const char *tag)
{
	uint8_t i = 0;
	while (tag[i])
	{
		if (i > 32)
			return (TAG_TOO_MANY_CHAR);

		if (!((tag[i] >= 'a' && tag[i] <= 'z')
				|| (tag[i] >= 'A' && tag[i] <= 'Z')
				|| (tag[i] >= '0' && tag[i] <= '9')
				|| (tag[i] == '-')
				|| (tag[i] == '_')))
			return (TAG_INVALID_CHAR);
	}
	return (TAG_ERROR_NONE);
}

void node_status(char *param)
{
	if (!is_empty_string(param))
	{
		uart_printstr("Error: Unknown command. No param for this command.\r\n");
		return ;
	}

	if (g_current_node.magic_nb != NODE_MAGIC_NUMBER)
		uart_printstr("Node unconfigured\r\n");

	uint16_t calculated_check
		= add_checksum_16((uint8_t *)&g_current_node, sizeof(t_node) - 2);

	if (calculated_check != g_current_node.integrity_check)
		uart_printstr("CRITICAL: Data corruption detected!\r\n");

	uart_printstr("Node ID: ");
	// uart_printhex32(g_current_node.node_id);
	uart_printstr("\r\nPriority: ");
	// uart_printint16(g_current_node.priority);
	uart_printstr("\r\nSlot: ");
	// uart_printint(g_current_node);
	uart_printstr("\r\nTag: \"");
	uart_printstr(g_current_node.tag);
	uart_printstr("\"\r\n");
}

void node_set_id(char *param)
{
	uint32_t new_id;

	if (is_empty_string(param))
	{
		uart_printstr("Error: Missing ID parameter.\r\n");
		return ;
	}

	if (!is_valid_uint32(param, &new_id))
	{
		uart_printstr("Error: Invalid ID format. Must be a 32-bit unsigned integer.\r\n");
		return ;
	}

	g_current_node.node_id = new_id;

	g_current_node.integrity_check
		= add_checksum_16((uint8_t *)&g_current_node, sizeof(t_node) - 2);

	if (eeprom_write_node(g_slot_addresses[g_current_slot], &g_current_node) != EEPROM_ERROR_NONE)
	{
		uart_printstr("Corruption detected.\r\n");
		eeprom_relocate_node(&g_current_node);
	}

	uart_printstr("Done.\r\n");
}

void node_set_prio(char *param)
{
	int16_t new_prio;

	if (is_empty_string(param))
	{
		uart_printstr("Error: Missing priority parameter.\r\n");
		return ;
	}

	if (!is_valid_int16(param, &new_prio))
	{
		uart_printstr("Error: Invalid priority format. Must be a 16-bit signed integer.\r\n");
		return ;
	}

	g_current_node.priority = new_prio;

	g_current_node.integrity_check
		= add_checksum_16((uint8_t *)&g_current_node, sizeof(t_node) - 2);

	if (eeprom_write_node(g_slot_addresses[g_current_slot], &g_current_node) != EEPROM_ERROR_NONE)
	{
		uart_printstr("Corruption detected.\r\n");
		eeprom_relocate_node(&g_current_node);
	}

	uart_printstr("Done.\r\n");
}

void node_set_tag(char *param)
{
	if (is_empty_string(param))
	{
		uart_printstr("Error: Missing tag parameter.\r\n");
		return ;
	}

	if (ft_strlen(param) > MAX_TAG_STR_LENGTH) 
	{
		uart_printstr("Error: Tag too long. Exceed maximum length\r\n");
		return ;
	}

	if (is_tag_valid(param) != TAG_ERROR_NONE) 
	{
		uart_printstr("Error: Invalid tag format. Must be alphanumeric with dashes/underscores.\r\n");
		return ;
	}

	uint8_t i = 0;
	while (i < MAX_TAG_STR_LENGTH && param[i] != '\0')
	{
		g_current_node.tag[i] = param[i];
		i++;
	}
	g_current_node.tag[i] = '\0';

	g_current_node.integrity_check
		= add_checksum_16((uint8_t *)&g_current_node, sizeof(t_node) - 2);

	if (eeprom_write_node(g_slot_addresses[g_current_slot], &g_current_node) != EEPROM_ERROR_NONE)
	{
		uart_printstr("Corruption detected.\r\n");
		eeprom_relocate_node(&g_current_node);
	}

	uart_printstr("Done.\r\n");
}

void node_factory_reset(char *param)
{
	if (!is_empty_string(param))
	{
		uart_printstr("Error: Unknown command. No param for this command.\r\n");
		return ;
	}

	// Invalidate current slot by clearing magic number
	t_node empty_node = {0};
	if (eeprom_write_node(g_slot_addresses[g_current_slot], &g_current_node) != EEPROM_ERROR_NONE)
	{
		uart_printstr("CRITICAL EEPROM FAILURE\r\n");
		return ;
	}

	uart_printstr("Done.\r\n");
}
