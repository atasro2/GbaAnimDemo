#ifndef GUARD_GLOBAL_H
#define GUARD_GLOBAL_H

#include <stddef.h>
#include "gba/gba.h"

#define ARRAY_COUNT(array) (size_t)(sizeof(array) / sizeof((array)[0]))
#define READ(ptr, type) (*(type*)(ptr))
#define READ_INC(ptr, type) ((-1)[(type *) ((ptr) = (void *) ((type *) (ptr) + 1))])
#define KEY_NEW() ({ (*(vu16 *)REG_ADDR_KEYINPUT) ^ KEYS_MASK; })
#define DivRoundNearest(divisor, dividend) (((divisor) + ((dividend)/2)) / (dividend))

#define LOAD_U32_LE(var, src) \
do { \
	(var) = *(src); \
	(var) |= (*((src) + 1) << 8); \
	(var) |= (*((src) + 2) << 16); \
	(var) |= (*((src) + 3) << 24); \
} while (0)

// Converts a number to Q8.8 fixed-point format
#define Q_8_8(n) ((s16)((n) * 256))

// Converts a number to Q4.12 fixed-point format
#define Q_4_12(n)  ((s16)((n) * 4096))
#define UQ_4_12(n)  ((u16)((n) * 4096))

// Converts a number to Q24.8 fixed-point format
#define Q_24_8(n)  ((s32)((n) * 256))

// Converts a Q8.8 fixed-point format number to a regular integer
#define Q_8_8_TO_INT(n) ((int)((n) / 256))

// Converts a Q4.12 fixed-point format number to a regular integer
#define Q_4_12_TO_INT(n)  ((int)((n) / 4096))
#define UQ_4_12_TO_INT(n)  ((int)((n) / 4096))

// Converts a Q24.8 fixed-point format number to a regular integer
#define Q_24_8_TO_INT(n) ((int)((n) >> 8))

// Rounding value for Q4.12 fixed-point format
#define Q_4_12_ROUND ((1) << (12 - 1))
#define UQ_4_12_ROUND ((1) << (12 - 1))


struct Joypad
{
	u8 newKeys;
	u8 heldKeys;
	u8 newKeysRaw;
	u8 heldKeysRaw;
};

struct Main {
	vu32 frameCounter;
	struct Joypad joypad;
};

extern struct Main gMain;

#endif//GUARD_GLOBAL_HP
