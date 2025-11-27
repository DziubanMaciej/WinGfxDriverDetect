#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define REPORT_INFO() std::cout
#define REPORT_ERROR() std::cerr << "ERROR: "
#define REPORT_WARNING() std::cerr << "WARNING: "

#include <windows.h>
#include <psapi.h>

#include <vector>
#include <iostream>
#include <algorithm>

#include "opengl.h"
#include "opencl.h"
#include "vulkan.h"
#include "dx11.h"
#include "dx12.h"
#include "known_drivers.h"

void tolower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

struct Module
{
    HMODULE handle;
    std::string fullPath;
    std::string name;
};

int getLoadedModules(std::vector<Module>& outModules)
{
    HANDLE process = GetCurrentProcess();

    DWORD neededSpace = 0;
    if (EnumProcessModules(process, nullptr, 0, &neededSpace) == 0)
    {
        REPORT_ERROR() << "EnumProcessModules failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
        return EXIT_FAILURE;
    }

    const DWORD modulesCount = neededSpace / sizeof(HMODULE);
    auto modules = std::make_unique<HMODULE[]>(modulesCount);
    if (EnumProcessModules(process, modules.get(), modulesCount * sizeof(HMODULE), &neededSpace) == 0)
    {
        REPORT_ERROR() << "EnumProcessModules failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
        return EXIT_FAILURE;
    }

    for (DWORD i = 0u; i < modulesCount; i++)
    {
        TCHAR fileName[MAX_PATH];
        if (GetModuleFileNameEx(process, modules[i], fileName, MAX_PATH) == 0)
        {
            REPORT_ERROR() << "GetModuleFileNameEx failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
            return EXIT_FAILURE;
        }

        TCHAR baseName[MAX_PATH];
        if (GetModuleBaseName(process, modules[i], baseName, MAX_PATH) == 0)
        {
            REPORT_ERROR() << "GetModuleFileNameEx failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
            return EXIT_FAILURE;
        }

        outModules.push_back(Module{modules[i], fileName, baseName});
    }

    return EXIT_SUCCESS;
}

int getDriverStorePath(const std::vector<Module> &modules, Api api, std::string &outDriverStorePath) {
    for (const Module &module : modules) {
        for (const KnownDriver &knownDriver : knownDrivers) {
            if (knownDriver.api != api)
            {
                continue;
            }

#if _WIN64
            const char* dllName = knownDriver.dllName64;
#else
            const char* dllName = knownDriver.dllName32;
#endif
            const char* moduleName = module.name.c_str();
            if (strcmp(dllName, moduleName) != 0)
            {
                continue;
            }

            const char* modulePath = module.fullPath.c_str();
            const size_t moduleDirectoryLength = strstr(modulePath, moduleName) - modulePath - 1; // -1 for trailing backslash
            outDriverStorePath.assign(modulePath, moduleDirectoryLength);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

void printNotFoundDriverHint(const std::vector<Module> &modulesBefore, const std::vector<Module> &modulesAfter)
{
    auto &out = REPORT_ERROR() << "No driver has been found.";
    const auto moduleCountDiff = int(modulesAfter.size()) - int(modulesBefore.size());
    if (moduleCountDiff > 0)
    {
        out << " The drivers are detected based on a hardcoded name, so it's possible GetDriverStore doesn't have the name stored. A total of "
            << moduleCountDiff << " new modules have been loaded after driver init. Maybe one of them is the driver?\n";
        int index = 0;
        for (const Module& module : modulesAfter)
        {
            bool isNew = true;
            for (const Module& moduleBefore : modulesBefore)
            {
                if (moduleBefore.handle == module.handle)
                {
                    isNew = false;
                }
            }

            if (isNew)
            {
                index++;
                out << "  " << index << ". " << module.fullPath;
                if (module.fullPath.find("DriverStore") != std::string::npos)
                {
                    out << " <--- This looks like a driver";
                }
                out << "\n";
            }
        }
    }
    else if (moduleCountDiff == 0)
    {
        out << " No new modules have been loaded after driver init. It must have failed silently or something else weird happened.\n";
    }
    else if (moduleCountDiff < 0)
    {
        out << " Some modules have been unloaded after driver init. This is weird.\n";
    }
}

int main(int argc, char **argv) {
    int res = EXIT_SUCCESS;
    Api api = Api::Vulkan;
    auto initFunc = &initVulkan;

    // Parse args
    if (argc > 1)
    {
        std::string argApi = argv[1];
        tolower(argApi);

        if (argApi == "opengl")
        {
            api = Api::OpenGL;
            initFunc = &initOpenGL;
        }
        else if (argApi == "opencl")
        {
            api = Api::OpenCL;
            initFunc = &initOpenCL;
        }
        else if (argApi == "vulkan")
        {
            api = Api::Vulkan;
            initFunc = &initVulkan;
        }
        else if (argApi == "dx11")
        {
            api = Api::Dx11;
            initFunc = &initDx11;
        }
        else if (argApi == "dx12")
        {
            api = Api::Dx12;
            initFunc = &initDx12;
        }
        else
        {
            REPORT_ERROR() << "Invalid API. Specify OpenGL, OpenCL, Vulkan, Dx11 or Dx12.\n";
            res = EXIT_FAILURE;
        }
    }

    // Init the driver and find its location
    std::vector<Module> modulesBefore = {};
    if (res == EXIT_SUCCESS)
    {
        res = getLoadedModules(modulesBefore);
    }
    if (res == EXIT_SUCCESS)
    {
        res = initFunc();
    }
    std::vector<Module> modulesAfter = {};
    if (res == EXIT_SUCCESS)
    {
        res = getLoadedModules(modulesAfter);
    }
    std::string driverStorePath = {};
    if (res == EXIT_SUCCESS)
    {
        res = getDriverStorePath(modulesAfter, api, driverStorePath);
        if (res != EXIT_SUCCESS)
        {
            printNotFoundDriverHint(modulesBefore, modulesAfter);
        }
    }
    if (res == EXIT_SUCCESS)
    {
        REPORT_INFO() << driverStorePath << std::endl;
    }

    return res;
}
