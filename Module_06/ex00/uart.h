#ifndef UART_H
# define UART_H

#include <avr/io.h>
#include "i2c_status_msg.h"

void uart_init(void);
void uart_tx(unsigned char c);
void uart_tx_uint10(uint16_t value);
void uart_printstr(const char* str);
void uart_display_status(uint8_t status_code);
char uart_rx(void);

#endif