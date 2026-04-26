#ifndef I2C_STATUS_MSG_H
# define I2C_STATUS_MSG_H

#include <util/twi.h>
#include <avr/io.h>

const char *i2c_status_desc(uint8_t status_code);

#endif