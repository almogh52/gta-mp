#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <subhook.h>

#include "section.h"
#include "address.h"

namespace gtamp
{
namespace hook
{
class manager
{
public:
	static void set_exe_memory(void *exe_memory);
	static void *get_exe_memory();

	static std::vector<section> get_sections();

	static bool install_hook(std::string hook_name, void *src, void *dst);
	static bool remove_hook(std::string hook_name);
	static address get_trampoline(std::string hook_name);

private:
	inline static std::unordered_map<std::string, std::shared_ptr<subhook::Hook>> _hooks;
	inline static void *exe_memory = 0;
};
}; // namespace hook
}; // namespace gtamp