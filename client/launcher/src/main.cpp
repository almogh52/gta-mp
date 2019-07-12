#include "launcher.h"

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			PSTR lpCmdLine, INT nCmdShow)
{
	gtamp::launcher::launcher launcher;

	launcher.run();

	return 0;
}