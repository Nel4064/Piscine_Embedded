#ifndef DELAY_US_AND_MS_H
# define DELAY_US_AND_MS_H

#include <avr/io.h>

void init_timer1_normal_mode(void);
void init_timer_for_delay(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

#endif