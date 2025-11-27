#pragma once

#if SUPPORT_OPENGL

int initOpenGL()
{
    // Create window class
    WNDCLASSA wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = "WinGfxDriverDetectClass";
    if (!RegisterClassA(&wc))
    {
        REPORT_ERROR() << "Failed to register window class.\n";
        return EXIT_FAILURE;
    }

    // Create window
    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, "WinGfxDriverDetectWindow", 0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
    if (!hwnd)
    {
        REPORT_ERROR() << "Failed to create dummy window.\n";
        return EXIT_FAILURE;
    }
    HDC deviceContext = GetDC(hwnd);
    if (!deviceContext)
    {
        REPORT_ERROR() << "Failed to get device context of dummy window.\n";
        return EXIT_FAILURE;
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
        0 };
    const int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    if (pixelFormat == 0)
    {
        REPORT_ERROR() << "Failed to choose pixel format for dummy window.\n";
        return EXIT_FAILURE;
    }
    if (SetPixelFormat(deviceContext, pixelFormat, &pfd) == 0)
    {
        REPORT_ERROR() << "Failed to set pixel format for dummy window.\n";
        return EXIT_FAILURE;
    }

    // Create GL context. We will not release it, to avoid any module unloading.
    HGLRC glContext = wglCreateContext(deviceContext);
    if (glContext == nullptr)
    {
        REPORT_ERROR() << "Failed to create GL context.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#else

int initOpenGL()
{
    REPORT_ERROR() << "OpenGL is not supported by GetDriverStore. Recompile the program with -DSUPPORT_OPENGL=1\n";
    return EXIT_FAILURE;
}

#endif
