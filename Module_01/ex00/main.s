	.file	"main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	main
	.type	main, @function
main:
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,8
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 8 */
/* stack size = 10 */
.L__stack_usage = 10
	ldi r24,lo8(36)
	ldi r25,0
	movw r30,r24
	ld r18,Z
	ldi r24,lo8(36)
	ldi r25,0
	ori r18,lo8(2)
	movw r30,r24
	st Z,r18
	std Y+1,__zero_reg__
	std Y+2,__zero_reg__
	std Y+3,__zero_reg__
	std Y+4,__zero_reg__
	ldi r24,lo8(64)
	ldi r25,lo8(13)
	ldi r26,lo8(3)
	ldi r27,0
	std Y+5,r24
	std Y+6,r25
	std Y+7,r26
	std Y+8,r27
.L3:
	ldd r24,Y+1
	ldd r25,Y+2
	ldd r26,Y+3
	ldd r27,Y+4
	adiw r24,1
	adc r26,__zero_reg__
	adc r27,__zero_reg__
	std Y+1,r24
	std Y+2,r25
	std Y+3,r26
	std Y+4,r27
	ldd r20,Y+1
	ldd r21,Y+2
	ldd r22,Y+3
	ldd r23,Y+4
	ldd r24,Y+5
	ldd r25,Y+6
	ldd r26,Y+7
	ldd r27,Y+8
	cp r20,r24
	cpc r21,r25
	cpc r22,r26
	cpc r23,r27
	brne .L3
	ldi r24,lo8(37)
	ldi r25,0
	movw r30,r24
	ld r19,Z
	ldi r24,lo8(37)
	ldi r25,0
	ldi r18,lo8(2)
	eor r18,r19
	movw r30,r24
	st Z,r18
	std Y+1,__zero_reg__
	std Y+2,__zero_reg__
	std Y+3,__zero_reg__
	std Y+4,__zero_reg__
	rjmp .L3
	.size	main, .-main
	.ident	"GCC: (Fedora 15.2.0-1.fc42) 15.2.0"
