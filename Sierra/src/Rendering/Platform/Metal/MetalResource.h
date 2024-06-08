//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../../RenderingResource.h"

#if defined(__OBJC__)
    #include <Metal/Metal.h>
    #include <QuartzCore/QuartzCore.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        template<typename T>
        struct id
        {
            id(T* data) : data(data) { }
            volatile T* data = nil;
        };
    }
#endif

namespace Sierra
{

    using MTLHandle = void*;

    class SIERRA_API MetalResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] GraphicsAPI GetAPI() const override { return GraphicsAPI::Metal; }

    protected:
        explicit MetalResource(const std::string_view name)
        {
            #if SR_ENABLE_LOGGING
                this->name = name;
            #endif
        }

    };

}
