F_CPU = 16000000UL
BAUD_RATE = 115200
MCU = atmega328p
PROGRAMMER = arduino

all: hex flash

hex:
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -o main.bin main.c
	avr-objcopy -O ihex main.bin main.hex

flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P /dev/ttyUSB0 -b $(BAUD_RATE) -U flash:w:main.hex:i

clean:
	rm -f *.hex *.bin

.PHONY: all hex flash clean