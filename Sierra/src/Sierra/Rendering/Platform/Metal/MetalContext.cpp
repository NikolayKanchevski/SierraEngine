//
// Created by Nikolay Kanchevski on 21.11.23.
//

// Define metal-cpp symbols
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "MetalContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalContext::MetalContext(const RenderingContextCreateInfo &createInfo)
        : RenderingContext(createInfo), device({ }, { })
    {
        SR_INFO("Metal context created successfully! Device in use: [{0}].", device.GetName());
    }

    /* --- DESTRUCTORS --- */

    void MetalContext::Destroy()
    {
        device.Destroy();
    }

}