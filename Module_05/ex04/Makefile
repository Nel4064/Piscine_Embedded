F_CPU = 16000000UL
BAUD_RATE = 115200
MCU = atmega328p
PROGRAMMER = arduino
PORT = /dev/ttyUSB0

SRC = main.c uart.c rgb_light.c
OBJ = $(SRC:.c=.o)
TARGET = main.bin

all: hex flash

hex: $(OBJ)
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -o main.bin main.c
	avr-objcopy -O ihex main.bin main.hex

%.o: %.c
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -c $< -o $@

flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD_RATE) -U flash:w:main.hex:i

clean:
	rm -f *.hex *.bin *.o

.PHONY: all hex flash clean screen



## Usefull command line in case of not closing properly port /dev/ttyUSB0.
# ps aux | grep tty
# kill -9 <pid>
# ls /dev/tty*