//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include "../../RenderingResource.h"

#if SR_ENABLE_LOGGING
    #define VK_VALIDATE(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) SR_ERROR("[Vulkan]: {0}() failed: {1} Error code: {2}.", std::string_view(#FUNCTION).substr(0, std::string_view(#FUNCTION).rfind('(')), MESSAGE, static_cast<int32>(result))
#else
    #define VK_VALIDATE(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
#endif


namespace Sierra
{

    class SIERRA_API VulkanResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline GraphicsAPI GetAPI() const override { return GraphicsAPI::Vulkan; };

    protected:
        static void PushToPNextChain(void* mainStruct, void* newStruct);

    };

}
