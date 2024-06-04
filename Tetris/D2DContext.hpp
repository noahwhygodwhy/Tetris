#pragma once


#include <cstddef>
#include "d2d1.h"
#include "Common.hpp"

#include <windowsx.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class D2DContext
{
public:
	D2DContext(HWND hwnd);
	~D2DContext();

	ComPtr<IDXGIAdapter4> dxgi_adapter;
	ComPtr<IDXGIFactory6> dxgi_factory;
	ComPtr<IDXGISwapChain4> dxgi_swapChain;
	//ComPtr<IDXGIDevice

	ComPtr<ID2D1Factory> d2d_factory;
	ComPtr<ID2D1HwndRenderTarget> d2d_renderTarget;

	RECT* rc;

	//ComPtr<ID3D1Device> device;

private:

};
