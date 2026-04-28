F_CPU = 16000000UL
BAUD_RATE = 115200
MCU = atmega328p
PROGRAMMER = arduino
PORT = /dev/ttyUSB0

SRC = main.c \
		uart.c \
		delay_us_and_ms.c \
		eeprom_rw.c \
		node_cmd.c \
		mini_libft.c \
		parsing.c

OBJ = $(SRC:.c=.o)
TARGET = main.bin

all: hex flash screen

hex: $(OBJ)
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -o $(TARGET) $(OBJ)
	avr-objcopy -O ihex main.bin main.hex

%.o: %.c
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -I. -c $< -o $@

flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD_RATE) -U flash:w:main.hex:i

dump:
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD_RATE) -U flash:r:main.dump

screen:
	screen $(PORT) $(BAUD_RATE)

clean:
	rm -f *.hex *.bin *.o *.dump

.PHONY: all hex flash clean screen



## Usefull command line in case of not closing properly port /dev/ttyUSB0.
# ps aux | grep tty
# kill -9 <pid>
# ls /dev/tty*