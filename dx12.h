#pragma once

#if SUPPORT_DX12

#include <d3d12.h>

int initDx12()
{
    ID3D12Device* device = nullptr;
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
    {
        REPORT_ERROR() << "D3D12CreateDevice failed";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#else

int initDx12()
{
    REPORT_ERROR() << "DirectX12 is not supported by GetDriverStore. Recompile the program with -DSUPPORT_DX12=1\n";
    return EXIT_FAILURE;
}

#endif
