#pragma once

#if SUPPORT_VULKAN

#include <vulkan/vulkan.h>

int initVulkan()
{
    VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = "GetDriverStore";
    appInfo.apiVersion = VK_API_VERSION_1_3;
    VkInstanceCreateInfo instanceInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instanceInfo.pApplicationInfo = &appInfo;
    VkInstance instance = nullptr;
    VkResult vkRes = vkCreateInstance(&instanceInfo, nullptr, &instance);
    if (vkRes != VK_SUCCESS)
    {
        REPORT_ERROR() << "vkCreateInstance failed";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#else

int initVulkan()
{
    REPORT_ERROR() << "Vulkan is not supported by GetDriverStore. Recompile the program with -DSUPPORT_VULKAN=1\n";
    return EXIT_FAILURE;
}

#endif
