	.include "asm/macros.inc"
	.syntax unified

    arm_func_start RLDecompress
RLDecompress:
    add r0, 1
    mov r3, 0
    ldrb r2, [r0], 1
    orr r3, r3, r2
    ldrb r2, [r0], 1
    orr r3, r3, r2, lsl #8
    ldrb r2, [r0], 1
    orr r3, r3, r2, lsl #16 @ DON'T OPTIMIZE THIS I WILL KILL YOU BECAUSE THIS IS SUPPOSED TO BE COMPATIBLE WITH THINGS THAT ARE NOT 4 BYTE ALIGNED JUST FOR THE SAKE OF IT SINCE I ALIGN MY SHIT
    add r2, r1, r3 @ R2 = Destination End Offset
.RLELoop:
    cmp r1, r2 @ Compare Destination Address To Destination End Offset
    beq .RLEEOF @ IF (Destination Address == Destination End Offset) RLEEOF
    ldrb r3, [r0], 1 @ R3 = RLE Flag Data (Bit 0..6 = Expanded Data Length: Uncompressed N-1, Compressed N-3, Bit 7 = Flag: 0 = Uncompressed, 1 = Compressed)
    ands r12, r3, 0x80 @ R4 = RLE Flag
    and r3, 0x7F @ T1 = Expanded Data Length
    add r3, 1 @ Expanded Data Length++
    bne .RLEDecode @ IF (BlockType != 0) RLE Decode Bytes
.RLECopy: @ ELSE Copy Uncompressed Bytes
    ldrb r12, [r0], 1 @ R4 = Byte To Copy
    strb r12, [r1], 1 @ Store Uncompressed Byte To Destination
    subs r3, 1 @ Expanded Data Length--
    bne .RLECopy @ IF (Expanded Data Length != 0) RLECopy
    b .RLELoop
.RLEDecode:
    add r3, 2 @ Expanded Data Length += 2
    ldrb r12, [r0], 1 @ R4 = Byte To Copy
.RLEDecodeByte:
	strb r12, [r1], 1 @ Store Uncompressed Byte To Destination
	subs r3, 1 @ Expanded Data Length--
	bne .RLEDecodeByte @ IF (Expanded Data Length != 0) RLEDecodeByte
	b .RLELoop
.RLEEOF:
    bx lr
    arm_func_end RLDecompress 
