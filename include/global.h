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