//
// Created by Nikolay Kanchevski on 4.10.24.
//

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "../DeviceErrors.h"

namespace Sierra
{

    void HandleVulkanError(const VkResult result, const std::string_view message)
    {
        if (result >= 0) return;

        switch (result)
        {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:                 throw DeviceOutOfMemory(message);
            default:                                            break;
        }

        throw UnknownDeviceError(message);
    }

}