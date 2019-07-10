#include "pe_loader.h"

#include <algorithm>
#include <fstream>

#include "../../shared/exception.h"

gtamp::pe_loader::pe_loader(FARPROC (*proc_resolver)(HMODULE, const char *))
	: _proc_resolver(proc_resolver) {}

gtamp::pe_loader::~pe_loader()
{
	if (_file_data)
	{
		delete[] _file_data;
	}
}

void gtamp::pe_loader::load_exe(std::string exe_path)
{
	std::ifstream exe_file;
	uint32_t exe_size;

	// Open the exe file for reading
	exe_file.open(exe_path, std::ios::binary | std::ios::in | std::ios::ate);

	// If open failed, throw exception
	if (!exe_file.is_open())
	{
		throw exception(EXECUTABLE_NOT_FOUND, "The executable of GTA V wasn't found in the requested path!");
	}

	// Get size of file
	exe_size = (uint32_t)exe_file.tellg();

	// Reset file
	exe_file.seekg(0, std::ios::beg);

	// Allocate memory for the file
	_file_data = new uint8_t[exe_size];

	// Copy the contents of the file
	exe_file.read((char *)_file_data, exe_size);

	// Close file
	exe_file.close();

	// Parse file
	parse_exe();
}

void gtamp::pe_loader::parse_exe()
{
	load_headers();
	load_sections();
	relocate();
	init_iat();
	apply_permissions();
	init_tls();
	load_exceptions_table();

	// Return the old NT headers
	memcpy(_nt_headers, _old_nt_headers, sizeof(IMAGE_NT_HEADERS) + (_old_nt_headers->FileHeader.NumberOfSections * (sizeof(IMAGE_SECTION_HEADER))));

	// Get the entry point
	_entry = std::function<void()>((void (*)())(_nt_headers->OptionalHeader.ImageBase + _nt_headers->OptionalHeader.AddressOfEntryPoint));
}

void gtamp::pe_loader::load_headers()
{
	char *binary_memory = nullptr;
	char *headers_buf = nullptr;

	// Get the DOS headers
	_dos_header = (PIMAGE_DOS_HEADER)(_file_data);
	if (_dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		throw exception(INVALID_DOS_SIGNATURE, "Invalid DOS Signature");
	}

	// Get the NT Headers
	_nt_headers = (PIMAGE_NT_HEADERS)(&_file_data[_dos_header->e_lfanew]);
	_old_nt_headers = (PIMAGE_NT_HEADERS)(&_file_data[_dos_header->e_lfanew]);
	if (_nt_headers->Signature != IMAGE_NT_SIGNATURE)
	{
		throw exception(INVALID_NT_SIGNATURE, "Invalid NT Signature");
	}

	// Verify machine for x64
	if (_nt_headers->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
	{
		throw exception(INVALID_EXECUTABLE, "The given executable doesn't support x64 architecture");
	}

	// Allocate memory for the binary in the wanted image base
	binary_memory = (char *)VirtualAlloc((LPVOID)_nt_headers->OptionalHeader.ImageBase, _nt_headers->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// If failed try to allocate memory in random position
	if (binary_memory == NULL)
	{
		binary_memory = (char *)VirtualAlloc(NULL, _nt_headers->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (binary_memory == NULL)
		{
			throw exception(OUT_OF_MEMORY, "Not enough memory");
		}
	}

	// Allocate buf for headers
	headers_buf = (char *)VirtualAlloc(binary_memory, _nt_headers->OptionalHeader.SizeOfHeaders, MEM_COMMIT, PAGE_READWRITE);
	if (headers_buf == NULL)
	{
		throw exception(UNKNOWN_ERROR, "Unable to allocate headers memory");
	}

	// Copy new headers to buf
	memcpy(headers_buf, _file_data, _old_nt_headers->OptionalHeader.SizeOfHeaders);

	// Set new NT headers with the new image base
	_nt_headers = (PIMAGE_NT_HEADERS) & ((const unsigned char *)(headers_buf))[_dos_header->e_lfanew];
	_nt_headers->OptionalHeader.ImageBase = (ULONGLONG)binary_memory;
}

void gtamp::pe_loader::load_sections()
{
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(_nt_headers);

	// Allocate each section and copy it's data
	for (int i = 0; i < _nt_headers->FileHeader.NumberOfSections; i++, section++)
	{
		char *dest;

		// If the section is empty
		if (section->SizeOfRawData == 0)
		{
			// If sections have alignment
			if (_nt_headers->OptionalHeader.SectionAlignment > 0)
			{
				// Allocate memory for alignment
				dest = (char *)VirtualAlloc((LPVOID)(_nt_headers->OptionalHeader.ImageBase + section->VirtualAddress), _nt_headers->OptionalHeader.SectionAlignment, MEM_COMMIT, PAGE_READWRITE);
				if (dest == NULL)
				{
					throw exception(UNKNOWN_ERROR, "Unable to allocate section memory");
				}

				// Set the physical address of the section
				section->Misc.PhysicalAddress = (DWORD)(ULONGLONG)dest;

				// Set null in the entire section
				memset(dest, 0, _nt_headers->OptionalHeader.SectionAlignment);
			}
		}
		else
		{
			// Allocate memory for section
			dest = (char *)VirtualAlloc((LPVOID)(_nt_headers->OptionalHeader.ImageBase + section->VirtualAddress), section->SizeOfRawData, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (dest == NULL)
			{
				throw exception(UNKNOWN_ERROR, "Unable to allocate section memory");
			}

			// Get the size of the section data
			uint32_t size_of_Data = std::min(section->SizeOfRawData, section->Misc.VirtualSize);

			// Copy the section data and set it's physical memory
			memcpy(dest, &_file_data[section->PointerToRawData], size_of_Data);
			section->Misc.PhysicalAddress = (DWORD)(ULONGLONG)dest;
		}
	}
}

void gtamp::pe_loader::relocate()
{
	// Get the delta from the wanted image base
	DWORD locationDelta = (DWORD)(_nt_headers->OptionalHeader.ImageBase - _old_nt_headers->OptionalHeader.ImageBase);

	// If the delta is bigger than 0
	if (locationDelta != 0)
	{
		// Get the relocation directory
		PIMAGE_DATA_DIRECTORY directory = &_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

		// Check if there are items to relocate
		if (directory->Size > 0)
		{
			// Get the start of the array of relocation blocks
			PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)(_nt_headers->OptionalHeader.ImageBase + directory->VirtualAddress);

			// While we didn't reach the end of the list of blocks
			while (relocation->VirtualAddress > 0)
			{
				char *dest = (char *)(_nt_headers->OptionalHeader.ImageBase + relocation->VirtualAddress);
				char *block_ptr = (char *)relocation + sizeof(IMAGE_BASE_RELOCATION);

				// For each reloaction in the block
				for (DWORD i = 0; i < ((relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2); i++, block_ptr++)
				{
					int type, offset;

					// Get the type and the offset of the relocation
					type = *block_ptr >> 12;
					offset = *block_ptr & 0xfff;

					// Relocate based on the the type of the relocation
					if (type == IMAGE_REL_BASED_HIGHLOW)
					{
						*(DWORD *)(dest + offset) += locationDelta;
					}
				}

				// Move to the next block of relocation
				relocation = (PIMAGE_BASE_RELOCATION)(((char *)relocation) + relocation->SizeOfBlock);
			}
		}
	}
}

void gtamp::pe_loader::init_iat()
{
	// Get the entry import directory
	PIMAGE_DATA_DIRECTORY directory = &_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	// Check if there are imports
	if (directory->Size > 0)
	{
		// Get the first import descriptor
		PIMAGE_IMPORT_DESCRIPTOR import_descriptor = (PIMAGE_IMPORT_DESCRIPTOR)(_nt_headers->OptionalHeader.ImageBase + directory->VirtualAddress);

		// Go through the library imports
		for (; !IsBadReadPtr(import_descriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && import_descriptor->Name; import_descriptor++)
		{
			ULONGLONG *thunk_ptr;
			FARPROC *func_ptr;

			// Get the lib name
			char *lib_name = (char *)(_nt_headers->OptionalHeader.ImageBase + import_descriptor->Name);

			// Try to load the library
			HMODULE handle = LoadLibraryA(lib_name);
			if (handle == NULL)
			{
				throw exception(LIBRARY_NOT_FOUND, "The library " + std::string(lib_name) + " cannot be found");
			}

			// Get the thunk ptr (ptr to Import Names Table) and the func ptr (ptr to Import Address Table)
			thunk_ptr = (ULONGLONG *)(_nt_headers->OptionalHeader.ImageBase + import_descriptor->OriginalFirstThunk);
			func_ptr = (FARPROC *)(_nt_headers->OptionalHeader.ImageBase + import_descriptor->FirstThunk);

			// For each import function
			for (; *thunk_ptr, *func_ptr; thunk_ptr++, func_ptr++)
			{
				// Check if the thunk is an hint for the function
				if (IMAGE_SNAP_BY_ORDINAL(*thunk_ptr))
					*func_ptr = GetProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunk_ptr));
				else
				{
					*func_ptr = _proc_resolver(handle, (LPCSTR)((PIMAGE_IMPORT_BY_NAME)(_nt_headers->OptionalHeader.ImageBase + (*thunk_ptr)))->Name);
				}
			}
		}
	}
}

void gtamp::pe_loader::apply_permissions()
{
	ULONGLONG imageOffset = (_nt_headers->OptionalHeader.ImageBase & 0xffffffff00000000);

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(_nt_headers);

	// Set permissions for each section
	for (int i = 0; i < _nt_headers->FileHeader.NumberOfSections; i++, section++)
	{
		bool executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		bool readable = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
		bool writeable = (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;
		bool no_cache = (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED) != 0;

		DWORD protect = get_protection(executable, writeable, readable, no_cache);

		// Get the size of the section
		DWORD size = section->SizeOfRawData;
		if (size == 0)
		{
			size = _nt_headers->OptionalHeader.SectionAlignment;
		}

		// Protect the section
		if (size > 0 && VirtualProtect((LPVOID)((ULONGLONG)section->Misc.PhysicalAddress | imageOffset), size, protect, &protect) == 0)
		{
			throw exception(SECTION_PROTECT_FAILED, "Cannot change section protect mode");
		}
	}
}

void gtamp::pe_loader::init_tls()
{
	// Get the data directory of TLS
	PIMAGE_DATA_DIRECTORY directory = &_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

	// Get this application's NT headers
	IMAGE_DOS_HEADER *source_dos_header = (IMAGE_DOS_HEADER *)GetModuleHandle(NULL);
	IMAGE_NT_HEADERS *source_nt_headers = (IMAGE_NT_HEADERS *)((char *)source_dos_header + source_dos_header->e_lfanew);

	// Get Target TLS and Source TLS
	const IMAGE_TLS_DIRECTORY *target_tls = (PIMAGE_TLS_DIRECTORY)((char *)source_dos_header + source_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
	const IMAGE_TLS_DIRECTORY *source_tls = (PIMAGE_TLS_DIRECTORY)(_nt_headers->OptionalHeader.ImageBase + directory->VirtualAddress);

	// Copy the TLS callbacks to the target TLS callbacks (this application)
	memcpy((void *)target_tls->StartAddressOfRawData, reinterpret_cast<void *>(source_tls->StartAddressOfRawData), source_tls->EndAddressOfRawData - source_tls->StartAddressOfRawData);

	// If has callback function
	if (directory->VirtualAddress > 0)
	{
		// Get the TLS directory and the first callback
		PIMAGE_TLS_DIRECTORY tls = (PIMAGE_TLS_DIRECTORY)(_nt_headers->OptionalHeader.ImageBase + directory->VirtualAddress);
		PIMAGE_TLS_CALLBACK *callback = (PIMAGE_TLS_CALLBACK *)tls->AddressOfCallBacks;

		// If callback is avaliable
		if (callback)
		{
			// Go through all callbacks and call each one with DLL_PROCESS_ATTCH
			while (*callback)
			{
				(*callback)((LPVOID)_nt_headers->OptionalHeader.ImageBase, DLL_PROCESS_ATTACH, NULL);
				callback++;
			}
		}
	}
}

void gtamp::pe_loader::load_exceptions_table()
{
	// Get the data directory of the exceptions table
	IMAGE_DATA_DIRECTORY *exceptionDirectory = &_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];

	// Get the function list of the excpetion handlers
	RUNTIME_FUNCTION *functionList = (RUNTIME_FUNCTION *)(_nt_headers->OptionalHeader.ImageBase + exceptionDirectory->VirtualAddress);
	DWORD entryCount = exceptionDirectory->Size / sizeof(RUNTIME_FUNCTION);

	// Try to add the excpetions functions table to the functions table
	if (!RtlAddFunctionTable(functionList, entryCount, (DWORD64)GetModuleHandle(NULL)))
	{
		throw exception(EXCEPTIONS_TABLE_LOAD_FAILED, "Failed to load the exceptions table");
	}
}

DWORD gtamp::pe_loader::get_protection(bool executable, bool writeable, bool readable, bool no_cache)
{
	DWORD protect = PAGE_NOACCESS;

	if (!executable)
		if (!readable)
			if (!writeable)
				protect = PAGE_NOACCESS;
			else
				protect = PAGE_WRITECOPY;
		else if (!writeable)
			protect = PAGE_READONLY;
		else
			protect = PAGE_READWRITE;
	else if (!readable)
		if (!writeable)
			protect = PAGE_EXECUTE;
		else
			protect = PAGE_EXECUTE_WRITECOPY;
	else if (!writeable)
		protect = PAGE_EXECUTE_READ;
	else
		protect = PAGE_EXECUTE_READWRITE;

	if (no_cache)
		protect |= PAGE_NOCACHE;

	return protect;
}

std::function<void()> gtamp::pe_loader::get_entry()
{
	return _entry;
}