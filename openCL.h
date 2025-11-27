#pragma once

#if SUPPORT_OPENCL

#include <CL/cl.h>

int initOpenCL()
{
    cl_uint numPlatforms{};
    if (clGetPlatformIDs(0, nullptr, &numPlatforms) != CL_SUCCESS)
    {
        REPORT_ERROR() << "clGetPlatformIDs failed";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#else

int initOpenCL()
{
    REPORT_ERROR() << "OpenCL is not supported by GetDriverStore. Recompile the program with -DSUPPORT_OPENCL=1\n";
    return EXIT_FAILURE;
}

#endif
