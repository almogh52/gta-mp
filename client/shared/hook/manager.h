#pragma once

#ifdef GTAMPCORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

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
class CORE_EXPORT manager
{
public:
	static void set_exe_memory(void *exe_memory);
	static void *get_exe_memory();

	static std::vector<section> get_sections();

	static bool install_hook(std::string hook_name, void *src, void *dst);
	static bool install_winapi_hook(std::string hook_name, HMODULE module, const char *proc_name, void *dst);
	static std::shared_ptr<subhook::Hook> get_hook_ptr(std::string hook_name);

	static bool remove_hook(std::string hook_name);
	static address get_trampoline(std::string hook_name);

private:
	inline static std::unordered_map<std::string, std::shared_ptr<subhook::Hook>> *_hooks = new std::unordered_map<std::string, std::shared_ptr<subhook::Hook>>();
	inline static void *exe_memory = 0;
};
}; // namespace hook
}; // namespace gtamp