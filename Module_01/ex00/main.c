#include <avr/io.h>

#ifndef F_CPU
 define F_CPU 16000000UL; // UL = suffix => unsigned long integer (= 32-bit integer here)
#endif

int main()
{
	DDRB |= (1 << DDB1); // PB1/D2 = OUTPUT
	uint32_t count = 0;
	uint32_t max_count = 200000;
	// // 42 cycles/iter at 16MHtz / 2 for 0.5s toggle
	// // F_CPU / 42 / 2; // = 190476
	// uint32_t max_count = F_CPU / 42 / 2;

	while (1)
	{
		count++;
		if (count == max_count)
		{
			PORTB ^= (1 << PORTB1);
			count = 0;
		}
	}

	return (0);
}

// ASM code with no -O optimisation at compile
// Main Loop .L3 => 42 Cycles
// .L3:
// ; --- increment count (Y+1..4) ---
// ldd r24, Y+1        ; 2 cyc  ┐
// ldd r25, Y+2        ; 2 cyc  │ load count (32-bit)
// ldd r26, Y+3        ; 2 cyc  │ from stack into r24:r27
// ldd r27, Y+4        ; 2 cyc  ┘
// adiw r24, 1         ; 2 cyc    add 1 to low 16 bits (r24:r25)
// adc r26, __zero_reg__; 1 cyc   propagate carry to byte 2
// adc r27, __zero_reg__; 1 cyc   propagate carry to byte 3
// std Y+1, r24        ; 2 cyc  ┐
// std Y+2, r25        ; 2 cyc  │ store count back
// std Y+3, r26        ; 2 cyc  │ to stack
// std Y+4, r27        ; 2 cyc  ┘

// ; --- compare count == max_count ---
// ldd r20, Y+1        ; 2 cyc  ┐
// ldd r21, Y+2        ; 2 cyc  │ reload count (redundant! -O0 doesn't reuse r24:r27)
// ldd r22, Y+3        ; 2 cyc  │
// ldd r23, Y+4        ; 2 cyc  ┘
// ldd r24, Y+5        ; 2 cyc  ┐
// ldd r25, Y+6        ; 2 cyc  │ load max_count
// ldd r26, Y+7        ; 2 cyc  │
// ldd r27, Y+8        ; 2 cyc  ┘
// cp  r20, r24        ; 1 cyc  ┐
// cpc r21, r25        ; 1 cyc  │ 32-bit compare
// cpc r22, r26        ; 1 cyc  │
// cpc r23, r27        ; 1 cyc  ┘
// brne .L3            ; 2 cyc    branch if not equal (taken = 2, not taken = 1)