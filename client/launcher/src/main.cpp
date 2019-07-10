#include "pe_loader.h"

#include <iostream>
#include <exception>
#include <thread>

#include "debug.h"

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			PSTR lpCmdLine, INT nCmdShow)
{
	gtamp::pe_loader loader = gtamp::pe_loader([](HMODULE module, const char *name) {
		DEBUG("Module: %d - Name: %s\n", module, name);

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

	GetCurrentDirectory(1000, idk);

	DEBUG("%s\n", idk);

	while (true)
	{
	}

	return 0;
}