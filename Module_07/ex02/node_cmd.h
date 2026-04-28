#ifndef NODE_CMD_H
# define NODE_CMD_H

#include "mini_libft.h"
#include "node.h"
#include "uart.h"
#include "eeprom_rw.h"

uint16_t	add_checksum_16(const uint8_t *data, uint16_t len);
uint8_t		is_node_valid(const t_node *node);
uint8_t		is_tag_valid(const char *tag);
void		node_status(char *param);
void		node_set_id(char *param);
void		node_set_prio(char *param);
void		node_set_tag(char *param);
void		node_factory_reset(char *param);

#endif