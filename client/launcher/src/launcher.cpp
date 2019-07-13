#include "launcher.h"

#include <Windows.h>
#include <thread>

#include "patches.h"
#include "../../shared/hook/manager.h"

LPCSTR __stdcall hook_GetCommandlineA()
{
	// Apply post load patches to the binary
	gtamp::launcher::patches::apply_post_load_patches();

	return GetCommandLineA();
}

FARPROC proc_handler(HMODULE module, const char *name)
{
	if (!strcmp(name, "GetCommandLineA"))
	{
		return (FARPROC)hook_GetCommandlineA;
	}

	return (FARPROC)GetProcAddress(module, name);
}

gtamp::launcher::launcher::launcher() : _loader(proc_handler) {}

void gtamp::launcher::launcher::run()
{
	std::string gta_dir = "D:\\Other\\Steam\\steamapps\\common\\Grand Theft Auto V";
	std::function<void()> entry;
	std::thread gta_initial_thread;

	// Apply pre load patches to allow debugging
	gtamp::launcher::patches::apply_pre_load_patches();

	// Set the current directory to the gta dir to load libraries from there
	SetCurrentDirectoryA(gta_dir.c_str());

	try
	{
		// Load the binary of GTA
		_loader.load_exe(gta_dir + "\\GTA5.exe");
	}
	catch (std::exception &ex)
	{
		MessageBoxA(NULL, ex.what(), "GTA:Multiplayer Error", MB_OK);
	}

	// Get the entry of the binary
	entry = _loader.get_entry();

	// Set the image base for the hook manager
	gtamp::hook::manager::set_exe_memory(_loader.get_image_base());

	// Create the initial thread for the GTA with it's entry and join it
	gta_initial_thread = std::thread(entry);
	gta_initial_thread.join();
}