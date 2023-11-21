//
// Created by Nikolay Kanchevski on 9.09.23.
//

#include "VulkanContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanContext::VulkanContext(const RenderingContextCreateInfo &createInfo)
        : RenderingContext(createInfo), instance(VulkanInstance({ })), device(VulkanDevice({ .instance = instance }, { }))
    {
        SR_INFO("Vulkan context created successfully! Device in use: [{0}].", device.GetName());
    }

    /* --- DESTRUCTOR --- */

    void VulkanContext::Destroy()
    {
        device.Destroy();
        instance.Destroy();
    }

}
