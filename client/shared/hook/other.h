#pragma once

#include <cstdint>

namespace gtamp
{
namespace hook
{
template <typename R, typename... Args>
static R func_call(uint64_t addr, Args... args)
{
	return ((R(*)(Args...))(addr))(args...);
}

}; // namespace hook
} // namespace gtamp