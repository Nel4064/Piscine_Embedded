#ifndef UART_H
# define UART_H

#include <avr/io.h>

void uart_init(void);
void uart_tx(unsigned char c);
void uart_tx_dec_uint10(uint16_t value);
void uart_tx_hex(uint8_t value);
void uart_printstr(const char* str);
char uart_rx(void);

#endif