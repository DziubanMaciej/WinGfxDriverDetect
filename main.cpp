#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <codecvt>
#include <locale>
#include <memory>
#include <psapi.h>

#include <iostream>

#define REPORT_INFO() std::cout
#define REPORT_ERROR() std::cerr << "ERROR: "
#define REPORT_WARNING() std::cerr << "WARNING: "

struct KnownDriver {
    const char *vendorName;
    const char *dllName;
};

const KnownDriver knownDrivers[] = {
#if _WIN64
    {"AMD", "atio6axx.dll"},
    {"NVIDIA", "nvoglv64.dll"},
#else
    {"AMD", "atioglxx.dll"},
    {"NVIDIA", "nvoglv32.dll"},
#endif
};

bool initGl() {
    // Create window class
    WNDCLASSA wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = "WinGfxDriverDetectClass";
    if (!RegisterClassA(&wc)) {
        REPORT_ERROR() << "Failed to register window class.\n";
        return false;
    }

    // Create window
    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, "WinGfxDriverDetectWindow", 0,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
    if (!hwnd) {
        REPORT_ERROR() << "Failed to create dummy window.\n";
        return false;
    }
    HDC deviceContext = GetDC(hwnd);
    if (!deviceContext) {
        REPORT_ERROR() << "Failed to get device context of dummy window.\n";
        return false;
    }

    // Set pixel format for window
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
        PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
        32,                                                         // Colordepth of the framebuffer.
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        24, // Number of bits for the depthbuffer
        8,  // Number of bits for the stencilbuffer
        0,  // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0,
        0,
        0};
    const int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    if (pixelFormat == 0) {
        REPORT_ERROR() << "Failed to choose pixel format for dummy window.\n";
        return false;
    }
    if (SetPixelFormat(deviceContext, pixelFormat, &pfd) == 0) {
        REPORT_ERROR() << "Failed to set pixel format for dummy window.\n";
        return false;
    }

    // Create GL context. We will not release it, to avoid any module unloading.
    HGLRC glContext = wglCreateContext(deviceContext);
    if (glContext == nullptr) {
        REPORT_ERROR() << "Failed to create GL context.\n";
        return false;
    }

    return true;
}

std::wstring getOpenglDriverStore() {
    if (!initGl()) {
        return {};
    }

    HANDLE process = GetCurrentProcess();

    DWORD neededSpace = 0;
    if (EnumProcessModules(process, nullptr, 0, &neededSpace) == 0) {
        REPORT_ERROR() << "EnumProcessModules failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
        return {};
    }

    const DWORD modulesCount = neededSpace / sizeof(HMODULE);
    auto modules = std::make_unique<HMODULE[]>(modulesCount);
    if (EnumProcessModules(process, modules.get(), modulesCount * sizeof(HMODULE), &neededSpace) == 0) {
        REPORT_ERROR() << "EnumProcessModules failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
        return {};
    }

    for (DWORD i = 0u; i < modulesCount; i++) {
        TCHAR moduleName[MAX_PATH];
        if (GetModuleFileNameEx(process, modules[i], moduleName, MAX_PATH) == 0) {
            REPORT_ERROR() << "GetModuleFileNameEx failed. GetLastError()=0x" << std::hex << GetLastError() << "\n";
            return {};
        }

        for (const KnownDriver &knownDriver : knownDrivers) {
            char *ptr = strstr(moduleName, knownDriver.dllName);
            if (ptr == nullptr) {
                continue;
            }

            size_t moduleDirLength = (ptr - moduleName) - 1; // -1 for trailing backslash
            auto moduleDir = std::make_unique<char[]>(moduleDirLength + 1);
            strncpy(moduleDir.get(), moduleName, moduleDirLength);
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wide = converter.from_bytes(moduleDir.get());
            return wide;
        }
    }

    REPORT_ERROR() << "No OpenGL driver found.\n";
    return {};
}

int main() {
    std::wstring actualStore = getOpenglDriverStore();
    if (actualStore.empty()) {
        return EXIT_FAILURE;
    }

    std::wcout << actualStore << '\n';
    return EXIT_SUCCESS;
}
