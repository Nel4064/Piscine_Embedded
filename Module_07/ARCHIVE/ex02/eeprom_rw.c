#include "eeprom_rw.h"

// DS40002061B-p.35 / Example EEPROM_read
uint8_t eeprom_read_byte(uint16_t address, uint8_t *data)
{
	if (address >= EEPROM_SIZE)
		return (EEPROM_INVALID_ADDR);

	// DS40002061B-p.31 Wait for completion of previous write
	uint16_t timeout = 10000;
	while ((EECR & (1 << EEPE)) && --timeout) // The user software can poll this bit and wait for a zero before writing the next byte
	{}
	
	if (timeout == 0)
		return (EEPROM_WRITE_VERIFY_FAILED);

	// DS40002061B-p.31 Set up address register
	EEARH = (address & 0x0300) >> 8;
	EEARL = (address & 0x00FF) >> 0;

	// DS40002061B-p.33 Start eeprom read by writing EERE 
	EECR |= (1 << EERE); // When the correct address is set up in the EEAR Register, the EERE bit must be written to a logic one to trigger the EEPROM read. 

	// DS40002061B-p.31 Return data from Data Register
	*data = EEDR; // For the EEPROM read operation, the EEDR contains the data read out from the EEPROM at the address given by EEAR

	return (EEPROM_ERROR_NONE);
}

// DS40002061B-p.34 / Example EEPROM_write
uint8_t eeprom_write_byte(uint16_t address, uint8_t data)
{
	if (address >= EEPROM_SIZE)
		return (EEPROM_INVALID_ADDR);

	// Read the current value at the address
	uint8_t current_data;
	eeprom_read_byte(address, &current_data);

	// If the current value is the same as the new value, do not write
	if (current_data == data)
		return (EEPROM_DATA_ALREADY_IN);

	// DS40002061B-p.31 Wait for completion of previous write
	uint16_t timeout = 10000;
	while ((EECR & (1 << EEPE)) && --timeout) // The user software can poll this bit and wait for a zero before writing the next byte
	{}
	
	if (timeout == 0)
		return (EEPROM_WRITE_VERIFY_FAILED);

	// DS40002061B-p.31 Set up address register
	EEARH = (address & 0x0300) >> 8;
	EEARL = (address & 0x00FF) >> 0;

	// DS40002061B-p.31 Write data to Data Register
	EEDR = data; // For the EEPROM write operation, the EEDR Register contains the data to be written to the EEPROM in the address given by the EEAR Register. 

	// DS40002061B-p.32 Write logical one to EEMPE
	EECR |= (1 << EEMPE) ; // The EEMPE bit determines whether setting EEPE to one causes the EEPROM to be written.

	// DS40002061B-p.32 Start eeprom write by setting EEPE
	EECR |= (1 << EEPE); // When address and data are correctly set up, the EEPE bit must be written to one to write the value into the EEPROM.

	// DS40002061B-p.31 Wait for completion of write
	timeout = 10000;
	while ((EECR & (1 << EEPE)) && --timeout)
	{}

	if (timeout == 0)
		return (EEPROM_WRITE_VERIFY_FAILED);

	return (EEPROM_ERROR_NONE);
}

void	eeprom_read_block(uint16_t address, uint8_t *data, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
		eeprom_read_byte(address + i, &data[i]);
}

void	eeprom_write_block(uint16_t address, const uint8_t *data, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
		eeprom_write_byte(address + i, data[i]);
}

bool	eeprom_node_structs_equal(const t_node *node1, const t_node *node2)
{
	return (ft_memcmp(node1, node2, sizeof(t_node)) == 0);
}

uint8_t eeprom_write_node(uint16_t address, const t_node *node)
{
	if (address >= EEPROM_SIZE || (address + NODE_SIZE) > EEPROM_SIZE)
		return (EEPROM_INVALID_ADDR);

	t_node	current_node;
	eeprom_read_block(address, (uint8_t *)&current_node, NODE_SIZE);

	// If the current node is uninitialized, treat it as a new node
	if (current_node.magic_nb != NODE_MAGIC_NUMBER)
	{
		// No need to compare or verify, just write the new node
		eeprom_write_block(address, (uint8_t *)node, NODE_SIZE);

		// Verify the write
		t_node verify_node;
		eeprom_read_block(address, (uint8_t *)&verify_node, NODE_SIZE);
		if (!eeprom_node_structs_equal(node, &verify_node))
			return (EEPROM_WRITE_VERIFY_FAILED);

		return (EEPROM_ERROR_NONE);
	}

	if (eeprom_node_structs_equal(node, &current_node))
		return (EEPROM_DATA_ALREADY_IN);

	eeprom_write_block(address, (uint8_t *)node, NODE_SIZE);

	t_node verify_node;
	eeprom_read_block(address, (uint8_t *)&verify_node, NODE_SIZE);

	if (!eeprom_node_structs_equal(node, &verify_node))
		return (EEPROM_WRITE_VERIFY_FAILED);

	return (EEPROM_ERROR_NONE);
}

uint8_t eeprom_relocate_node(t_node *node)
{
	static uint8_t current_slot = 0;

	for (uint8_t i = 0; i < NUM_SLOTS; i++)
	{
		uint8_t result = eeprom_write_node(g_slot_addresses[current_slot], node);

		if (result == EEPROM_ERROR_NONE || result == EEPROM_DATA_ALREADY_IN)
		{
			g_current_slot = current_slot;
			return (EEPROM_ERROR_NONE);
		}

		// Mark the current slot as invalid by clearing magic_nb
		t_node invalid_node = {0};
		eeprom_write_block(g_slot_addresses[current_slot], (uint8_t *)&invalid_node, NODE_SIZE);

		current_slot = (current_slot + 1) % NUM_SLOTS;
	}
	return (EEPROM_RELOCATION_FAILED);  // All slots failed
}


// void	eeprom_hexdump_slots(void)
// {
// 	uint8_t data;

// 	for (uint8_t addr = 0; addr < g_slot_address_max ; addr += HEXDUMP_BYTES_PER_LINE)
// 	{
// 		// Display EEPROM Address / dec
// 		uart_tx_dec_uint10(addr);
// 		uart_tx(' ');
// 		uart_tx('-');
// 		uart_tx(' ');

// 		// Display EEPROM Data / hex
// 		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
// 		{
// 			eeprom_read_byte(addr + i, &data);
// 			uart_tx_hex_uint8(data);
// 			uart_tx(' ');
// 		}
// 		uart_tx('|');

// 		// Display EEPROM Data / char
// 		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
// 		{
// 			eeprom_read_byte(addr + i, &data);
// 			if (data >= ' ' && data < 127)
// 				uart_tx(data);
// 			else
// 				uart_tx('.');
// 		}
// 		uart_tx('|');
// 		uart_tx('\r');
// 		uart_tx('\n');
// 	}
// }

void eeprom_hexdump_slots(void)
{
	uint8_t data;
	const char *slot_colors[NUM_SLOTS] =
	{
		COLOR_CYAN,  // Slot 0: No color
		COLOR_BLUE,   // Slot 1: Blue
		COLOR_CYAN,  // Slot 2: No color
		COLOR_BLUE    // Slot 3: Blue
	};

	for (uint8_t addr = 0; addr < g_slot_address_max; addr += HEXDUMP_BYTES_PER_LINE)
	{
		// Display EEPROM Address in decimal
		uart_tx_dec_uint10(addr);
		uart_tx(' ');
		uart_tx('-');
		uart_tx(' ');

		uint8_t slot = 0;

		// Display EEPROM Data in hex
		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			uint8_t current_addr = addr + i;

			// Determine which slot the current address belongs to
			while (slot < NUM_SLOTS && current_addr >= g_slot_addresses[slot + 1])
				slot++;

			// Apply slot color
			if (current_addr < g_slot_address_max)
				uart_printstr(slot_colors[slot]);
			eeprom_read_byte(current_addr, &data);
			uart_tx_hex_uint8(data);
			uart_tx(' ');
			uart_printstr(COLOR_RESET); // Reset color after each byte
		}
		uart_tx('|');

		// Display EEPROM Data in ASCII
		for (uint8_t i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			uint8_t current_addr = addr + i;
			eeprom_read_byte(current_addr, &data);
			if (data >= ' ' && data < 127)
				uart_tx(data);
			else
				uart_tx('.');
		}
		uart_tx('|');
		uart_tx('\r');
		uart_tx('\n');
	}
}
