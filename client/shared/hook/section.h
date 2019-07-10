#pragma once

#include <cstdint>

namespace gtamp
{
namespace hook
{
struct section
{
	uintptr_t addr;
	uint64_t size;
};
}; // namespace hook
}; // namespace gtamp