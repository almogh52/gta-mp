#include "modify.h"

#include <cstring>

void gtamp::hook::modify::put(void *dst, uint8_t opcode, uint64_t amount)
{
	for (int i = 0; i < amount; i++)
	{
		*((uint8_t *)dst + i) = opcode;
	}
}

void gtamp::hook::modify::put(void *dst, uint8_t opcode)
{
	*(uint8_t *)dst = opcode;
}

void gtamp::hook::modify::put(void *dst, std::vector<uint8_t> vec)
{
	for (auto iterator = vec.begin(); iterator != vec.end(); iterator++)
	{
		*((uint8_t *)dst + (vec.end() - iterator)) = *iterator;
	}
}

void gtamp::hook::modify::nop(void *dst)
{
	put(dst, 0x90);
}

void gtamp::hook::modify::nop(void *dst, uint64_t amount)
{
	put(dst, 0x90, amount);
}

void gtamp::hook::modify::ret(void *dst, uint8_t params_size = 0)
{
	if (!params_size)
	{
		put(dst, 0xC3);
	} else {
		put(dst, 0xC2);
		put((char *)dst + 1, params_size);
	}
}