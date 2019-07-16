#include "launcher.h"

#include <Windows.h>
#include <thread>

#include <client.h>

#include "splash_screen.h"
#include "patches.h"
#include "../../shared/hook/manager.h"

LPCSTR __stdcall hook_GetCommandlineA()
{
	// Apply post load patches to the binary
	spdlog::get("Launcher")->info("Applying Post-Load patches..");
	gtamp::launcher::patches::apply_post_load_patches();

	// Close the splash screen
	gtamp::launcher::splash_screen::close();

	return GetCommandLineA();
}

BOOL __stdcall hook_ShowWindow(HWND hWnd, int nCmdShow)
{
	// Set name of the window
	SetWindowTextA(hWnd, "GTA:Multiplayer");

	return ShowWindow(hWnd, nCmdShow);
}

void __stdcall hook_OutputDebugString(const char *str)
{
	spdlog::get("Launcher")->info("GTA V Debug: {}", str);
}

FARPROC proc_handler(HMODULE module, const char *name)
{
	spdlog::get("Launcher")->info("GetProcAddress: Name - {}", name);

	if (!strcmp(name, "GetCommandLineA"))
	{
		return (FARPROC)hook_GetCommandlineA;
	} else if (!strcmp(name, "ShowWindow"))
	{
		return (FARPROC)hook_ShowWindow;
	} else if (!strcmp(name, "OutputDebugStringA") || !strcmp(name, "OutputDebugStringW"))
	{
		return (FARPROC)hook_OutputDebugString;
	}

	return (FARPROC)GetProcAddress(module, name);
}

gtamp::launcher::launcher::launcher()
	: _logger(create_logger("Launcher")), _loader(proc_handler) {}

void gtamp::launcher::launcher::run()
{
	std::string gta_dir = "D:\\Other\\Steam\\steamapps\\common\\Grand Theft Auto V";
	std::function<void()> entry;
	std::thread gta_initial_thread;

	client client;

	splash_screen::show();

	// Flush log every second to file
	spdlog::flush_every(std::chrono::seconds(1));

	// Apply pre load patches to allow debugging
	_logger->info("Applying Pre-Load patches..");
	gtamp::launcher::patches::apply_pre_load_patches();

	// Set the current directory to the gta dir to load libraries from there
	SetCurrentDirectoryA(gta_dir.c_str());

	try
	{
		// Load the binary of GTA
		_logger->info("Loading GTA binary file..");
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
	_logger->info("Starting GTA's initial thread..");
	gta_initial_thread = std::thread(entry);
	gta_initial_thread.detach();

	// Run the client core
	client.run();
}