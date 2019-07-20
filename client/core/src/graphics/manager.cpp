#include "manager.h"

#include "../../include/core.h"

#include "../../../shared/hook/manager.h"
#include "../../../shared/hook/pattern.h"

gtamp::core::graphics::manager::manager(core *core) : manager_interface("Graphics", core), _directx(new gtamp::core::graphics::directx) {}

void gtamp::core::graphics::manager::init()
{
	// Init the DirectX handler
	_directx->init();

	// Post-Launch event
	_core->event_manager()->on(event::POST_LAUNCH, []() {
		// Install a hook for the present function of DirectX11
		hook::manager::install_hook("DirectX11Present",
									(uint8_t *)hook::pattern("E9 ? ? ? ? 40 55 53 56 57 41 54 41 56") + 5,
									+[](UINT SyncInterval, UINT Flags) -> HRESULT {
										return hook::manager::get_trampoline("DirectX11Present").call<HRESULT, UINT, UINT>(SyncInterval, Flags);
									});
	});
}