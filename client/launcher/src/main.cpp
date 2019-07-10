#include "pe_loader.h"

#include <iostream>
#include <exception>
#include <thread>

#include "../../shared/debug.h"
#include "../../shared/hook/hook_manager.h"
#include "../../shared/hook/pattern.h"

LPCSTR __stdcall hook_GetCommandlineA()
{
	DEBUG("%x\n", gtamp::hook::pattern("E8 ? ? ? ? 84 C0 75 ? B2 01 B9 2F A9 C2 F4").get_matches()[0]);

	return GetCommandLineA();
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			PSTR lpCmdLine, INT nCmdShow)
{
	gtamp::pe_loader loader = gtamp::pe_loader([](HMODULE module, const char *name) {
		DEBUG("Module: %d - Name: %s\n", module, name);

		if (!strcmp(name, "GetCommandLineA")) {
			return (FARPROC)hook_GetCommandlineA;
		}

		return (FARPROC)GetProcAddress(module, name);
	});

	DEBUG("Starting\n");

	char idk[1000];
	GetCurrentDirectory(1000, idk);
	SetCurrentDirectory("D:\\Other\\Steam\\steamapps\\common\\Grand Theft Auto V");

	try
	{
		loader.load_exe("D:\\Other\\Steam\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe");
	}
	catch (std::exception &ex)
	{
		DEBUG(ex.what())
		return 1;
	}

	//ApplyGTAVPreLaunchPatches();

	SetCurrentDirectory(idk);

	std::function<void()> entry = loader.get_entry();

	std::thread gta_initial_thread = std::thread([entry]() -> void {
		SetCurrentDirectory("D:\\Other\\Steam\\steamapps\\common\\Grand Theft Auto V");
		entry();
	});
	gta_initial_thread.detach();

	gtamp::hook::hook_manager::set_exe_memory(loader.get_image_base());

	GetCurrentDirectory(1000, idk);

	DEBUG("%s\n", idk);

	while (true)
	{
	}

	return 0;
}