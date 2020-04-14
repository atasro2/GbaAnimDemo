	.include "asm/macros.inc"
	.syntax unified

_start:
	b _init
rom_header:
	.include "asm/rom_header.inc"
_init:
	mov r0, #0x12
	msr cpsr_fc, r0
	ldr sp, =0x02038000
	mov r0, #0x1f
	msr cpsr_fc, r0
	ldr sp, =0x02040000
	ldr r0, =_intr
	ldr r1, =0x03007FFC
	str r0, [r1]
	ldr r0, =AgbMain+1
	bx r0
	b _start
	
	.pool
