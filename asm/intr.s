	.include "asm/macros.inc"
	.syntax unified
	@ hand optimized compiler generated code
	
	ARM_FUNC_START _intr
_intr:
	mov	r12, #0x4000000
	add r12, r12, #0x200
	ldr	r2, [r12]
	and	r2, r2, r2, lsr #16
	mov	r3, #0
	mov	r1, #1
loop:
	ands	r0, r2, r1, lsl r3
	bne callInterrupt
	add	r3, r3, #1
	cmp	r3, #14
	bne	loop
callInterrupt:
	ldr	r2, =gIntrTable
	strh r0, [r12, #2]
	ldr	r3, [r2, r3, lsl #2]
	bx	r3

	.pool
