#pragma once

#if SUPPORT_DX11

#include <d3d11.h>

int initDx11()
{
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &device,
        nullptr,
        &context
    );

    if (FAILED(hr))
    {
        REPORT_ERROR() << "D3D11CreateDevice failed";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#else

int initDx11()
{
    REPORT_ERROR() << "DirectX11 is not supported by GetDriverStore. Recompile the program with -DSUPPORT_DX11=1\n";
    return EXIT_FAILURE;
}

#endif
