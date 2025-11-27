#pragma once

enum class Vendor
{
    Intel,
    Nvidia,
    Amd,
};

enum class Api
{
    OpenGL,
    OpenCL,
    Vulkan,
    Dx11,
    Dx12,
};

struct KnownDriver
{
    Vendor vendor;
    Api api;
    const char* dllName32;
    const char* dllName64;
};

const KnownDriver knownDrivers[] = {
    { Vendor::Amd,    Api::OpenGL, "atioglxx.dll", "atio6axx.dll" },
    { Vendor::Amd,    Api::OpenCL, "amdocl32.dll", "amdocl64.dll" },
    //{ Vendor::Amd,    Api::Vulkan, "amdvlk32.dll", "amdvlk64.dll" },
    { Vendor::Amd,    Api::Dx11,   "amdxx32.dll",  "amdxx64.dll"  },
    { Vendor::Amd,    Api::Dx12,   "amdxc64.dll",  "amdxc64.dll"  },

    { Vendor::Nvidia, Api::OpenGL, "nvoglv32.dll", "nvoglv64.dll" },
};
