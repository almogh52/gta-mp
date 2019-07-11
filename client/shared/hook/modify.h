#pragma once

#include <cstdint>
#include <vector>

namespace gtamp
{
namespace hook
{
class modify
{
public:
	static void put(void *dst, uint8_t opcode, uint64_t amount);
	static void put(void *dst, uint8_t opcode);
	static void put(void *dst, std::vector<uint8_t> vec);

	static void nop(void *dst);
	static void nop(void *dst, uint64_t amount);

	static void ret(void *dst, uint8_t params_size = 0);
};
}; // namespace hook
}; // namespace gtamp