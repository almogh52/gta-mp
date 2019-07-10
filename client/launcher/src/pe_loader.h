#pragma once

#include <cstdint>
#include <functional>

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

namespace gtamp
{
class pe_loader
{
public:
	pe_loader(FARPROC(*proc_resolver)(HMODULE, const char *));
	~pe_loader();

	void load_exe(std::string exe_path);

	std::function<void()> get_entry();

private:
	FARPROC(*_proc_resolver)(HMODULE, const char *);

	uint8_t *_file_data;

	PIMAGE_DOS_HEADER _dos_header;
	PIMAGE_NT_HEADERS _old_nt_headers;
	PIMAGE_NT_HEADERS _nt_headers;

	std::function<void()> _entry;

	void parse_exe();
	
	void load_headers();
	void load_sections();
	void relocate();
	void init_iat();
	void apply_permissions();
	void init_tls();
	void load_exceptions_table();

	DWORD get_protection(bool executable, bool writeable, bool readable, bool no_cache);
};
}; // namespace gtamp