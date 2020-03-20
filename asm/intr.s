	.include "asm/macros.inc"
	.syntax unified
	@ hand optimized compiler generated code (looks nothing like compiler code now lol)
	@ credit goes to ax6 and Gericom + a bunch of other people i might have forgotten 

	ARM_FUNC_START _intr
_intr:
	mov r3, 0x4000000
	ldr r2, [r3, 0x200]!
	and r2, r2, r2, lsr #16
	str r2, [r3, -0x208]
	mov r1, #-1
.loop:
	add r1, #1
	lsrs r2, #1
	bcc .loop
	ldr r2, =gIntrTable
	mov r0, #1
	lsl r0, r1
	strh r0, [r3, #2]
	ldr r0, [r2, r1, lsl #2]
	bx r0

	.pool

	.global interrupt_end
interrupt_end:
