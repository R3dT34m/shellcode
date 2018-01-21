	.arch armv6
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 4
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"gimli.c"
	.text
	.align	2
	.global	gimli
	.syntax unified
	.arm
	.fpu vfp
	.type	gimli, %function
gimli:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	add	r4, r0, #12
	ldr	ip, .L9
	ldr	r5, .L9+4
.L2:
	sub	r2, r0, #4
.L3:
	ldr	r3, [r2, #20] [j + 4]
	ldr	r1, [r2, # 4] [j]
	ldr	lr, [r2, #36] [j + 8]
	ror	r3, r3, #23
	ror	r1, r1, #8
	and	r7, r3, lr
	eor	r6, r1, lr, lsl #1
	eor	r6, r6, r7, lsl #2
	str	r6, [r2, #36]     
	orr	r7, r1, lr
	eor	r6, r1, r3
	and	r1, r1, r3
	eor	r3, r3, lr
	eor	r6, r6, r7, lsl #1
	eor	r3, r3, r1, lsl #3
	str	r6, [r2, #20]
	str	r3, [r2, #4]!
	cmp	r4, r2
	bne	.L3
  
	ands	r3, ip, #3
	bne	.L4
  
	ldr	r3, [r0]
	ldr	r2, [r0, #4]
	ldr	r1, [r0, #12]
	str	r3, [r0, #4]
	ldr	r3, [r0, #8]
	eor	r2, r2, ip
	str	r1, [r0, #8]
	str	r3, [r0, #12]
	str	r2, [r0]
.L4:
	cmp	r3, #2
	bne	.L5
  
	ldr	r3, [r0]
	ldr	r2, [r0, #8]
	str	r3, [r0, #8]
	str	r2, [r0]
	ldr	r3, [r0, #4]
	ldr	r2, [r0, #12]
	str	r3, [r0, #12]
	str	r2, [r0, #4]
.L5:
	sub	ip, ip, #1
	cmp	ip, r5
	bne	.L2
	pop	{r4, r5, r6, r7, pc}
.L10:
	.align	2
.L9:
	.word	-1640531688
	.word	-1640531712
	.size	gimli, .-gimli
	.ident	"GCC: (Raspbian 6.3.0-18+rpi1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",%progbits
