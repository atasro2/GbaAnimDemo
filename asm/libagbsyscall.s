	.include "asm/macros.inc"
	.syntax unified

	THUMB_FUNC_START CpuSet
CpuSet:
	svc #0xb
	bx lr

	THUMB_FUNC_START LZ77UnCompWram
LZ77UnCompWram:
	svc #0x11
	bx lr

	THUMB_FUNC_START RegisterRamReset
RegisterRamReset:
	svc #1
	bx lr

	thumb_func_start VBlankIntrWait
VBlankIntrWait:
	movs r2, 0
	swi 0x5
	bx lr
	thumb_func_end VBlankIntrWait

	thumb_func_start Div
Div:
	swi 0x6
	bx lr
	thumb_func_end Div
