#include "modify.h"

#include <cstring>

void gtamp::hook::modify::put(void *dst, uint8_t opcode, uint64_t amount)
{
	for (int i = 0; i < amount; i++) {
		*((uint8_t *)dst + i) = opcode;
	}
}

void gtamp::hook::modify::put(void *dst, uint8_t opcode)
{
	*(uint8_t *)dst = opcode;
}