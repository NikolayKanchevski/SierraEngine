//
// Created by Nikolay Kanchevski on 9.09.23.
//

#include "RenderingContext.h"

#if defined(SR_VULKAN_SUPPORTED)
    #include "Platform/Vulkan/VulkanContext.h"
#endif
#if defined(SR_METAL_SUPPORTED)
    #include "Platform/Metal/MetalContext.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RenderingContext::RenderingContext(const RenderingContextCreateInfo &createInfo)
    {

    }

    std::unique_ptr<RenderingContext> RenderingContext::Create(const RenderingContextCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.graphicsAPI ==+ GraphicsAPI::Undefined, "Cannot create a rendering context with graphics API set to [{0}]!", createInfo.graphicsAPI._to_string());
        switch (createInfo.graphicsAPI)
        {
            case GraphicsAPI::Vulkan:
            {
                #if !defined(SR_VULKAN_SUPPORTED)
                    SR_ERROR("Cannot create rendering context using the Vulkan API, as it is unsupported on the system, or the CMake option [\"SIERRA_BUILD_VULKAN\"] hast not been turned on!");
                    return nullptr;
                #else
                    return std::make_unique<VulkanContext>(createInfo);
                #endif
            }
            case GraphicsAPI::DirectX:
            {
                SR_ERROR("Cannot create rendering context using DirectX, as it has not been implemented yet!");
                return nullptr;
            }
            case GraphicsAPI::Metal:
            {
                #if !defined(SR_METAL_SUPPORTED)
                    SR_ERROR("Cannot create rendering context using the Metal API, as it is unsupported on the system!");
                    return nullptr;
                #else
                    return std::make_unique<MetalContext>(createInfo);
                #endif
            }
            case GraphicsAPI::OpenGL:
            {
                SR_ERROR("Cannot create rendering context using OpenGL, as it has not been implemented yet!");
                return nullptr;
            }
            default:
            case GraphicsAPI::Undefined:
            {
                SR_ERROR("Cannot create rendering context using an undefined GraphicsAPI!");
                return nullptr;
            }
        }
    }

    /* --- DESTRUCTOR --- */

    void RenderingContext::Destroy()
    {

    }

}