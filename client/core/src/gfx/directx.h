#pragma once

#include <Windows.h>
#include <d3d11.h>

namespace gtamp
{
namespace core
{
namespace gfx
{
class directx
{
public:
	void init();

	void set_device(ID3D11Device *device)
	{
		_device = device;
	}

	void set_swap_chain(IDXGISwapChain *swap_chain)
	{
		_swap_chain = swap_chain;
	}

private:
	IDXGISwapChain *_swap_chain;
	ID3D11Device *_device;
};
}; // namespace graphics
}; // namespace core
}; // namespace gtamp