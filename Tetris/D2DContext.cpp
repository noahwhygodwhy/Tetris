#include "D2DContext.hpp"

D2DContext::D2DContext(HWND hwnd)
{

    D2D1_FACTORY_OPTIONS options = {};

    ComPtr<IDXGIFactory4> factory4;
    TIF(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory4)));
    TIF(factory4->QueryInterface(IID_PPV_ARGS(&dxgi_factory)));
    factory4->Release();
    TIF(this->dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgi_adapter)));


    TIF(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory),
            &options,
            &d2d_factory
        )
    );

    rc = new RECT();

    GetClientRect(hwnd, rc);


    HRESULT hr = d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            hwnd,
            D2D1::SizeU(
                rc->right - rc->left,
                rc->bottom - rc->top)
        ),
        &d2d_renderTarget
    );
}

D2DContext::~D2DContext()
{
}