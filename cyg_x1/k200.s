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
	.file	"k200.c"
	.text
	.align	2
	.global	k200_permute
	.syntax unified
	.arm
	.fpu vfp
	.type	k200_permute, %function
k200_permute:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, lr}
	sub	sp, sp, #68
	ldr	r2, .L21
	add	ip, sp, #20
	mov	r3, r0
	add	r5, r2, #16
	mov	lr, ip
.L2:
	ldr	r0, [r2]	@ unaligned
	ldr	r1, [r2, #4]	@ unaligned
	mov	r4, ip
	add	r2, r2, #8
	cmp	r2, r5
	stmia	r4!, {r0, r1}
	mov	ip, r4
	bne	.L2
	ldrh	r2, [r2]	@ unaligned
	add	ip, sp, #40
	strh	r2, [r4]	@ unaligned
	ldr	r2, .L21+4
	mov	r4, ip
	add	r7, r2, #24
	mov	r6, r7
.L3:
	ldr	r0, [r2]	@ unaligned
	ldr	r1, [r2, #4]	@ unaligned
	mov	r5, ip
	add	r2, r2, #8
	cmp	r2, r7
	stmia	r5!, {r0, r1}
	mov	ip, r5
	bne	.L3
	ldr	r1, [r6, #4]	@ unaligned
	ldr	r0, [r6]	@ unaligned
	add	r2, sp, #8
	add	ip, sp, #8
	stmia	r2!, {r0, r1}
	ldrb	r1, [r6, #8]	@ zero_extendqisi2
	strb	r1, [r2]
	mov	r1, #0
	mov	r5, r1
.L4:
	sub	r0, r3, #1
	mov	r6, #0
.L5:
	ldrb	r2, [r0, #1]!	@ zero_extendqisi2
	ldrb	r7, [r0, #5]	@ zero_extendqisi2
	eor	r2, r2, r7
	ldrb	r7, [r0, #10]	@ zero_extendqisi2
	eor	r2, r2, r7
	ldrb	r7, [r0, #15]	@ zero_extendqisi2
	eor	r2, r2, r7
	ldrb	r7, [r0, #20]	@ zero_extendqisi2
	eor	r2, r2, r7
	strb	r2, [sp, r6]
	add	r6, r6, #1
	cmp	r6, #5
	bne	.L5
	mov	r2, #0
.L7:
	add	r0, ip, r2
	add	r7, r2, #1
	ldrb	r0, [r0, #4]	@ zero_extendqisi2
	add	r6, sp, #64
	add	r8, sp, #64
	add	r0, r6, r0
	ldrb	r6, [ip, r7]	@ zero_extendqisi2
	ldrb	r0, [r0, #-64]	@ zero_extendqisi2
	add	r6, r8, r6
	ldrb	r8, [r6, #-64]	@ zero_extendqisi2
	lsr	r6, r8, #7
	orr	r6, r6, r8, lsl #1
	eor	r0, r0, r6
	uxtb	r0, r0
.L6:
	ldrb	r6, [r3, r2]	@ zero_extendqisi2
	eor	r6, r6, r0
	strb	r6, [r3, r2]
	add	r2, r2, #5
	cmp	r2, #24
	bls	.L6
	cmp	r7, #5
	mov	r2, r7
	bne	.L7
	ldrb	r7, [r3, #1]	@ zero_extendqisi2
	mov	r0, #1
	mov	r6, r5
.L8:
	add	r6, r6, r0
	uxtb	r6, r6
	and	r8, r6, #7
	rsb	r2, r8, #0
	and	r2, r2, #7
	lsr	r2, r7, r2
	orr	r2, r2, r7, lsl r8
	add	r7, r4, r0
	add	r0, r0, #1
	ldrb	r8, [r7, #-1]	@ zero_extendqisi2
	cmp	r0, #25
	ldrb	r7, [r3, r8]	@ zero_extendqisi2
	strb	r2, [r3, r8]
	bne	.L8
	mov	r6, #0
.L10:
	add	r2, r3, r6
	mov	r7, #0
	ldr	r0, [r2]	@ unaligned
	str	r0, [sp]
	ldrb	r0, [r2, #4]	@ zero_extendqisi2
	strb	r0, [sp, #4]
.L9:
	add	r7, r7, #1
	mov	r8, ip
	add	r9, sp, #64
	ldrb	r0, [r8, r7]!	@ zero_extendqisi2
	cmp	r7, #5
	add	r0, r9, r0
	ldrb	r8, [r8, #1]	@ zero_extendqisi2
	ldrb	r9, [r0, #-64]	@ zero_extendqisi2
	add	r0, sp, #64
	add	r8, r0, r8
	ldrb	r0, [r8, #-64]	@ zero_extendqisi2
	ldrb	r8, [r2]	@ zero_extendqisi2
	bic	r0, r0, r9
	eor	r0, r0, r8
	strb	r0, [r2], #1
	bne	.L9
	add	r6, r6, #5
	cmp	r6, #25
	bne	.L10
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldrb	r0, [lr, r1]	@ zero_extendqisi2
	add	r1, r1, #1
	cmp	r1, #18
	eor	r2, r2, r0
	strb	r2, [r3]
	bne	.L4
	add	sp, sp, #68
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, pc}
.L22:
	.align	2
.L21:
	.word	.LANCHOR0
	.word	.LANCHOR0+18
	.size	k200_permute, .-k200_permute
	.section	.rodata
	.set	.LANCHOR0,. + 0
.LC0:
	.byte	1
	.byte	-126
	.byte	-118
	.byte	0
	.byte	-117
	.byte	1
	.byte	-127
	.byte	9
	.byte	-118
	.byte	-120
	.byte	9
	.byte	10
	.byte	-117
	.byte	-117
	.byte	-119
	.byte	3
	.byte	2
	.byte	-128
.LC1:
	.byte	10
	.byte	7
	.byte	11
	.byte	17
	.byte	18
	.byte	3
	.byte	5
	.byte	16
	.byte	8
	.byte	21
	.byte	24
	.byte	4
	.byte	15
	.byte	23
	.byte	19
	.byte	13
	.byte	12
	.byte	2
	.byte	20
	.byte	14
	.byte	22
	.byte	9
	.byte	6
	.byte	1
.LC2:
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.ident	"GCC: (Raspbian 6.3.0-18+rpi1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",%progbits
