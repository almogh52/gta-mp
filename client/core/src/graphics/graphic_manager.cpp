#include "graphic_manager.h"

#include <windows.h>

#include "../../../shared/hook/manager.h"
#include "../../../shared/hook/pattern.h"

void gtamp::client::graphic_manager::init()
{
	// Install a hook for the present function of DirectX11
	hook::manager::install_hook("DirectX11Present",
								(uint8_t *)hook::pattern("E9 ? ? ? ? 40 55 53 56 57 41 54 41 56") + 5,
								+[](UINT SyncInterval, UINT Flags) -> HRESULT {
									return hook::manager::get_trampoline("DirectX11Present").call<HRESULT, UINT, UINT>(SyncInterval, Flags);
								});
}