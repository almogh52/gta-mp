#include "manager.h"

#include <algorithm>

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include "../exception.h"

#ifdef GTAMPCORE_EXPORT
void gtamp::hook::manager::set_exe_memory(void *exe_memory)
{
	gtamp::hook::manager::exe_memory = exe_memory;
}

void *gtamp::hook::manager::get_exe_memory()
{
	return gtamp::hook::manager::exe_memory;
}

std::vector<gtamp::hook::section> gtamp::hook::manager::get_sections()
{
	std::vector<section> sections;

	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS nt_headers;
	PIMAGE_SECTION_HEADER section;

	dos_header = (PIMAGE_DOS_HEADER)(gtamp::hook::manager::exe_memory);
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		throw exception(INVALID_DOS_SIGNATURE, "Invalid DOS Signature");
	}

	// Get the NT Headers
	nt_headers = (PIMAGE_NT_HEADERS)(&((char *)gtamp::hook::manager::exe_memory)[dos_header->e_lfanew]);
	if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
	{
		throw exception(INVALID_NT_SIGNATURE, "Invalid NT Signature");
	}

	// Get the first section
	section = IMAGE_FIRST_SECTION(nt_headers);

	// For each section init it's struct
	for (int i = 0; i < nt_headers->FileHeader.NumberOfSections; i++, section++)
	{
		bool executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		bool readable = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;

		if (readable && executable)
		{
			auto beg = (nt_headers->OptionalHeader.ImageBase + section->VirtualAddress);

			uint32_t sizeOfData = std::min(section->SizeOfRawData, section->Misc.VirtualSize);

			gtamp::hook::section sec = {beg, sizeOfData};

			sections.push_back(sec);
		}
	}

	return sections;
}

bool gtamp::hook::manager::install_hook(std::string hook_name, void *src, void *dst)
{
	bool success = false;
	std::shared_ptr<subhook::Hook> hook = std::make_shared<subhook::Hook>();

	// Try to install the hook
	success = hook->Install(src, dst, subhook::HookFlag64BitOffset);

	// Add the hook to the map of hooks
	_hooks->insert({hook_name, hook});

	return success;
}

bool gtamp::hook::manager::remove_hook(std::string hook_name)
{
	std::shared_ptr<subhook::Hook> hook;

	try
	{
		// Find the hook
		hook = _hooks->find(hook_name)->second;

		// Remove the hook
		_hooks->erase(hook_name);
	}
	catch (...)
	{
		throw exception(HOOK_NOT_FOUND, "The hook " + hook_name + " wasn't found");
	}

	return hook->Remove();
}

gtamp::hook::address gtamp::hook::manager::get_trampoline(std::string hook_name)
{
	std::shared_ptr<subhook::Hook> hook;

	try
	{
		// Find the hook
		hook = _hooks->find(hook_name)->second;
	}
	catch (...)
	{
		throw exception(HOOK_NOT_FOUND, "The hook " + hook_name + " wasn't found");
	}

	return address((uintptr_t)hook->GetTrampoline());
}
#endif