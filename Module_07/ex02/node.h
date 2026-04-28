#ifndef NODE_H
#define NODE_H

#define MAX_TAG_STR_LENGTH	((uint8_t)32)
#define NODE_MAGIC_NUMBER	((uint16_t)0x4242) // Arbitrary choice
#define NUM_SLOTS			((uint8_t)4)
#define NODE_SIZE			sizeof(t_node) // = 43 with current struct for t_node

#define TRUE  				((uint8_t)0)
#define FALSE 				((uint8_t)1)

#define MAX_USER_STR_LENGTH		((uint8_t)50)

#define PARSING_ERROR_NONE		((uint8_t)0)
#define PARSING_INVALID_CHAR	((uint8_t)1)
#define PARSING_TOO_MANY_CHAR	((uint8_t)2)

#define TAG_ERROR_NONE			((uint8_t)0)
#define TAG_INVALID_CHAR		((uint8_t)1)
#define TAG_TOO_MANY_CHAR		((uint8_t)2)

#define NODE_ERROR_NONE			((uint8_t)0)
#define MAGIC_NB_UNMATCH		((uint8_t)1)
#define INTEGRITY_CHECK_FAILED	((uint8_t)2)

#define EEPROM_ERROR_NONE		    ((uint8_t)0)
#define EEPROM_INVALID_ADDR         ((uint8_t)1)
#define EEPROM_DATA_ALREADY_IN	    ((uint8_t)2)
#define EEPROM_WRITE_VERIFY_FAILED	((uint8_t)3)
#define EEPROM_RELOCATION_FAILED    ((uint8_t)4)
#define EEPROM_NO_VALID_SLOT        ((uint8_t)5)

#define	CMD_STATUS				((uint8_t)0)
#define	CMD_SET_ID				((uint8_t)1)
#define	CMD_SET_PRIO			((uint8_t)2)
#define	CMD_SET_TAG				((uint8_t)3)
#define FACTORY_RESET			((uint8_t)4)

typedef struct s_node
{
	uint16_t	magic_nb;		// Must equal NODE_MAGIC_NUMBER
	uint32_t	node_id;		// 32-bit Node ID
	int16_t 	priority;		// 16-bit Priority
	char		tag[MAX_TAG_STR_LENGTH + 1]; // 32-character Tag + '/0'
	uint16_t	integrity_check;// CRC-16 or other checksum
}	t_node;						// = 43 bytes with current struct for t_node

extern const uint16_t g_slot_addresses[NUM_SLOTS];
extern t_node	g_current_node;
extern uint8_t	g_current_slot;

#endif