#pragma once

#include <cstdint>
#include <vector>

#include "section.h"

namespace gtamp
{
namespace hook
{
class hook_manager
{
public:
	static void set_exe_memory(void *exe_memory);
	static void *get_exe_memory();

	static std::vector<section> get_sections();

private:
	inline static void *exe_memory = 0;
};
}; // namespace hook
}; // namespace gtamp