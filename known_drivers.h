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
    { Vendor::Amd,    Api::OpenGL, "atioglxx.dll",    "atio6axx.dll"    },
    { Vendor::Amd,    Api::OpenCL, "amdocl32.dll",    "amdocl64.dll"    },
    { Vendor::Amd,    Api::Vulkan, "amdvlk32.dll",    "amdvlk64.dll"    },
    { Vendor::Amd,    Api::Dx11,   "amdxx32.dll",     "amdxx64.dll"     },
    { Vendor::Amd,    Api::Dx12,   "amdxc64.dll",     "amdxc64.dll"     },

    { Vendor::Nvidia, Api::OpenGL, "nvoglv32.dll",    "nvoglv64.dll"    },
    { Vendor::Nvidia, Api::OpenCL, "nvgpucomp32",     "nvgpucomp64.dll" },
    { Vendor::Nvidia, Api::Vulkan, "nvoglv32",        "nvoglv64.dll"    },
    { Vendor::Nvidia, Api::Dx11,   "nvldumd.dll",     "nvldumdx.dll"    },
    { Vendor::Nvidia, Api::Dx12,   "nvldumd.dll",     "nvldumdx.dll"    },

    { Vendor::Intel,  Api::OpenGL, "ig11icd32.dll",   "ig11icd64.dll"   },
    { Vendor::Intel,  Api::OpenCL, "igdrcl32.dll",    "igdrcl64.dll"    },
    { Vendor::Intel,  Api::Vulkan, "igvk32.dll",      "igvk64.dll"      },
    { Vendor::Intel,  Api::Dx11,   "igd10iumd32.dll", "igd10iumd64.dll" },
    { Vendor::Intel,  Api::Dx12,   "igd12umd32.dll",  "igd12umd64.dll"  },
};
