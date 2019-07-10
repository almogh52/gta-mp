#pragma once

#include <cstdint>

namespace gtamp
{
namespace hook
{
class modify
{
public:
	static void put(void *dst, uint8_t opcode, uint64_t amount);
	static void put(void *dst, uint8_t opcode);
};
}; // namespace hook
}; // namespace gtamp