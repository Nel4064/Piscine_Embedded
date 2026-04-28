#ifndef EEPROM_RW_H
# define EEPROM_RW_H

#include <avr/io.h>
#include "uart.h"
#include "mini_libft.h"
#include "node.h"

#define EEPROM_SIZE				    ((uint8_t)1024)	
#define HEXDUMP_BYTES_PER_LINE      ((uint8_t)16)

uint8_t eeprom_read_byte(uint16_t address, uint8_t *data);
uint8_t eeprom_write_byte(uint16_t address, uint8_t data);
void    eeprom_read_block(uint16_t address, uint8_t *data, uint16_t len);
void    eeprom_write_block(uint16_t address, const uint8_t *data, uint16_t len);
bool	eeprom_node_structs_equal(const t_node *node1, const t_node *node2);
uint8_t eeprom_write_node(uint16_t address, const t_node *node);
uint8_t eeprom_relocate_node(t_node *node);
void    eeprom_hexdump(void);
void    eeprom_hexdump_with_highlight(uint16_t highlight_addr);

#endif