#include "directx.h"

#include <spdlog/spdlog.h>

#include "../../include/core.h"
#include "../../../shared/hook/manager.h"

void gtamp::core::gfx::directx::init()
{
	HMODULE directx = LoadLibraryA("d3d11.dll");

	hook::manager::install_winapi_hook("DirectX11Hook", directx, "D3D11CreateDeviceAndSwapChain", +[](IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain, ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext) -> HRESULT {
		HRESULT res = hook::manager::get_trampoline("DirectX11Hook").call<HRESULT, IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, const DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **>(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

		// If the create was successful
		if (res == S_OK)
		{
			// Set the created device and swap chain
			core::core::get()->graphics_manager()->directx()->set_device(*ppDevice);
			core::core::get()->graphics_manager()->directx()->set_swap_chain(*ppSwapChain);
		}

		return res;
	});
}