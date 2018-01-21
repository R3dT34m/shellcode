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
	.file	"k800.c"
	.text
	.align	2
	.global	rc
	.syntax unified
	.arm
	.fpu vfp
	.type	rc, %function
rc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrsb	r3, [r0]
	mov	r2, #1
	mov	r1, #0
	mov	ip, r2
.L11:
	tst	r2, #128
	beq	.L18
	strb	r3, [r0]
	mov	r0, r1
	bx	lr
.L6:
	tst	r3, #1
	beq	.L3
	cmp	r2, #32
	bhi	.L3
.L14:
	sub	lr, r2, #1
	eor	r1, r1, ip, lsl lr
.L3:
	lsl	r2, r2, #1
	cmp	r3, #0
	lsl	r3, r3, #1
	eorlt	r3, r3, #113
	uxtb	r2, r2
	tst	r2, #128
	sxtb	r3, r3
	beq	.L6
	strb	r3, [r0]
	mov	r0, r1
	ldr	pc, [sp], #4
.L18:
	tst	r3, #1
	beq	.L13
	cmp	r2, #32
	strls	lr, [sp, #-4]!
	bls	.L14
.L13:
	cmp	r3, #0
	lsl	r3, r3, #1
	eorlt	r3, r3, #113
	lsl	r2, r2, #1
	sxtb	r3, r3
	uxtb	r2, r2
	b	.L11
	.size	rc, .-rc
	.align	2
	.global	k800_permute
	.syntax unified
	.arm
	.fpu vfp
	.type	k800_permute, %function
k800_permute:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
	sub	sp, sp, #64
	mov	r3, #1
	strb	r3, [sp, #7]
	ldr	r3, .L41
	add	r2, sp, #40
	add	r5, r3, #24
	mov	r4, r0
	mov	r8, r2
	mov	lr, r5
.L22:
	ldr	r0, [r3]	@ unaligned
	ldr	r1, [r3, #4]	@ unaligned
	mov	ip, r2
	add	r3, r3, #8
	cmp	r3, r5
	stmia	ip!, {r0, r1}
	mov	r2, ip
	bne	.L22
	ldr	r0, [lr]	@ unaligned
	ldr	r1, [lr, #4]	@ unaligned
	ldrb	r2, [lr, #8]	@ zero_extendqisi2
	add	r3, sp, #8
	mov	r5, #22
	add	r6, sp, #8
	add	r7, sp, #20
	stmia	r3!, {r0, r1}
	strb	r2, [r3]
.L23:
	sub	r2, r4, #4
	mov	r1, #0
.L24:
	ldr	r3, [r2, #4]!
	ldr	r0, [r2, #20]
	eor	r3, r3, r0
	ldr	r0, [r2, #40]
	eor	r3, r3, r0
	ldr	r0, [r2, #60]
	eor	r3, r3, r0
	ldr	r0, [r2, #80]
	eor	r3, r3, r0
	str	r3, [r7, r1, lsl #2]
	add	r1, r1, #1
	cmp	r1, #5
	bne	.L24
  
	mov	r3, #0
.L26:
	add	r2, r6, r3
	add	r1, sp, #64
	ldrb	r2, [r2, #4]	@ zero_extendqisi2
	add	r0, r3, #1
	add	ip, sp, #64
	add	r2, r1, r2, lsl #2
	ldr	r1, [r2, #-44]
	ldrb	r2, [r6, r0]	@ zero_extendqisi2
	add	r2, ip, r2, lsl #2
	ldr	r2, [r2, #-44]
	eor	r1, r1, r2, ror #31
.L25:
	ldr	r2, [r4, r3, lsl #2]
	eor	r2, r2, r1
	str	r2, [r4, r3, lsl #2]
	add	r3, r3, #5
	cmp	r3, #24
	bls	.L25
  
	cmp	r0, #5
	mov	r3, r0
	bne	.L26
	
  ldr	ip, [r4, #4]
	mov	r3, #1
	mov	r1, #0
.L27:
	add	r1, r1, r3
	add	r0, r8, r3
	uxtb	r1, r1
	and	r2, r1, #31
	ldrb	r0, [r0, #-1]	@ zero_extendqisi2
	rsb	r2, r2, #32
	add	r3, r3, #1
	ror	r2, ip, r2
	cmp	r3, #25
	ldr	ip, [r4, r0, lsl #2]
	str	r2, [r4, r0, lsl #2]
	bne	.L27
  
	mov	r3, r4
	add	r9, r4, #100
.L30:
	mov	r2, r3
	mov	ip, r7
	add	r10, r3, #16
.L28:
	ldr	r0, [r2]	@ unaligned
	ldr	r1, [r2, #4]	@ unaligned
	mov	lr, ip
	add	r2, r2, #8
	cmp	r2, r10
	stmia	lr!, {r0, r1}
	mov	ip, lr
	bne	.L28
	ldr	r0, [r2]	@ unaligned
	mov	r1, r3
	mov	r2, #0
	str	r0, [lr]
.L29:
	add	r2, r2, #1
	mov	lr, r6
	add	ip, sp, #64
	ldrb	r0, [lr, r2]!	@ zero_extendqisi2
	cmp	r2, #5
	ldrb	lr, [lr, #1]	@ zero_extendqisi2
	add	r0, ip, r0, lsl #2
	ldr	ip, [r0, #-44]
	add	r0, sp, #64
	add	lr, r0, lr, lsl #2
	ldr	r0, [lr, #-44]
	bic	ip, r0, ip
	ldr	r0, [r1]
	eor	r0, r0, ip
	str	r0, [r1], #4
	bne	.L29
	add	r3, r3, #20
	cmp	r9, r3
	bne	.L30
	add	r0, sp, #7
	bl	rc
	ldr	r3, [r4]
	subs	r5, r5, #1
	eor	r0, r0, r3
	str	r0, [r4]
	bne	.L23
	add	sp, sp, #64
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L42:
	.align	2
.L41:
	.word	.LANCHOR0
	.size	k800_permute, .-k800_permute
	.section	.rodata
	.set	.LANCHOR0,. + 0
.LC0:
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
.LC1:
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
