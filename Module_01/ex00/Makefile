F_CPU = 16000000UL
BAUD_RATE = 115200
MCU = atmega328p
PROGRAMMER = arduino

all: hex flash

hex:
	avr-gcc -Wall -Wextra -Werror -mmcu=$(MCU) -DF_CPU=$(F_CPU) -o main.bin main.c
	avr-objcopy -O ihex main.bin main.hex

ASM:
	avr-gcc -Wall -Wextra -Werror -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -S -o main.s main.c

flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P /dev/ttyUSB0 -b $(BAUD_RATE) -U flash:w:main.hex:i

clean:
	rm -f *.hex *.bin

.PHONY: all hex flash clean

# Flags for avr-gcc:
# avr-gcc is the C compiler for AVR microcontrollers, used to compile C code into machine code.
# -mmcu=$(MCU): Tells the compiler to generate optimized code for the ATmega328P architecture.
# -DF_CPU=$(F_CPU): Sets F_CPU lo define clock frequency to enable delay functions,
# UART communication, and PWM generation (16 MHz, cf. quartz)
# -Os: Optimizes the code for size, which is critical for microcontrollers with limited flash.
# -c : Compiles only (no link)

# Flags for avr-objcopy:
# avr-objcopy converts binary files into Intel HEX format for flashing.
# The Intel HEX format is a standardized way to represent binary data as ASCII text,
# making it easier to transmit and store (https://en.wikipedia.org/wiki/Intel_HEX)
# -O ihex: Outputs the file in Intel HEX format, required for flashing tools like avrdude.
# -R .eeprom: Excludes the EEPROM section, ensuring only the flash memory is programmed.

# Flags for avrdude:
# avrdude is the tool used to upload compiled code to the microcontroller's flash memory.
# -F: Forces writing to flash, even if the microcontroller doesn't respond as expected.
# -c $(PROGRAMMER): Specifies the programmer hardware (e.g., "arduino").
# -p $(MCU): Specifies the microcontroller model (e.g., "atmega328p").
# -P /dev/ttyUSB0: Specifies the serial port for communication with the programmer.
# -b $(BAUD_RATE): Sets the baud rate for serial communication (e.g., 115200).
# -U flash:w:main.hex:i Updates the flash memory with the contents of main.hex.

