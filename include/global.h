#ifndef GUARD_GLOBAL_H
#define GUARD_GLOBAL_H

#include <stddef.h>
#include "gba/gba.h"

#define ARRAY_COUNT(array) (size_t)(sizeof(array) / sizeof((array)[0]))

#define KEY_NEW() ({ (*(u16 *)REG_ADDR_KEYINPUT) ^ KEYS_MASK; })

#endif//GUARD_GLOBAL_H