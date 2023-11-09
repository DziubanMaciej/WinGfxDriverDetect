#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <codecvt>
#include <locale>
#include <memory>
#include <psapi.h>

#include <iostream>

#define FATAL_ERROR(message)                  \
    {                                         \
        printf("FATAL ERROR: %s\n", message); \
        throw 1;                              \
    }

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

void initGl() {
    // Create window class
    WNDCLASSA wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = "Dummy_WGL_djuasioadaddwa";
    if (!RegisterClassA(&wc)) {
        FATAL_ERROR("Failed to register dummy OpenGL window.");
    }

    // Create window
    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, "Dummy OpenGL Window", 0,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
    if (!hwnd) {
        FATAL_ERROR("Failed to create dummy OpenGL window.");
    }
    HDC deviceContext = GetDC(hwnd);

    // Create pixel format
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
    int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    if (!SetPixelFormat(deviceContext, pixelFormat, &pfd)) {
        FATAL_ERROR("Pixel format failed");
    }

    // Create GL context
    HGLRC glContext = wglCreateContext(deviceContext);
}

std::wstring getOpenglDriverStore() {
    initGl();

    HANDLE process = GetCurrentProcess();

    DWORD neededSpace = 0;
    if (EnumProcessModules(process, nullptr, 0, &neededSpace) == 0) {
        return L"";
    }

    const DWORD modulesCount = neededSpace / sizeof(HMODULE);
    auto modules = std::make_unique<HMODULE[]>(modulesCount);
    if (EnumProcessModules(process, modules.get(), modulesCount * sizeof(HMODULE),
                           &neededSpace) == 0) {
        return L"";
    }

    for (DWORD i = 0u; i < modulesCount; i++) {
        TCHAR moduleName[MAX_PATH];
        if (GetModuleFileNameEx(process, modules[i], moduleName, MAX_PATH) == 0) {
            return L"";
        }

        for (const KnownDriver &knownDriver : knownDrivers) {
            char *ptr = strstr(moduleName, knownDriver.dllName);
            if (ptr != nullptr) {
                size_t moduleDirLength =
                    (ptr - moduleName) - 1; // -1 for trailing backslash
                auto moduleDir = std::make_unique<char[]>(moduleDirLength + 1);
                strncpy(moduleDir.get(), moduleName, moduleDirLength);
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(moduleDir.get());
                return wide;
            }
        }
    }

    return L"";
}

bool compareStringCaseInsensitive(const std::wstring &a,
                                  const std::wstring &b) {
    const size_t sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

int main() {
    std::wstring actualStore = getOpenglDriverStore();
    if (!actualStore.empty()) {
        std::wcout << actualStore << '\n';
        return 0;
    } else {
        std::wcerr << "No OpenGL driver found.";
        return 3;
    }
}
