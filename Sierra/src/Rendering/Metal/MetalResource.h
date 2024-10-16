//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#if defined(__OBJC__)
    #include <Metal/Metal.h>
    #include <QuartzCore/QuartzCore.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using MTLResource = void;

        template<typename T>
        struct id
        {
            id(T* data) : data(data) { }
            volatile T* data = nil;
        };
    }
#endif

#include "../RenderingResource.h"

namespace Sierra
{

    /* --- TYPE DEFINITIONS --- */
    using MTLResource = void;

    class SIERRA_API MetalResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] RenderingBackendType GetBackendType() const noexcept override { return RenderingBackendType::Metal; }

        /* --- COPY SEMANTICS --- */
        MetalResource(const MetalResource&) = delete;
        MetalResource& operator=(const MetalResource&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalResource(MetalResource&&) = delete;
        MetalResource& operator=(MetalResource&&) = delete;

        /* --- DESTRUCTORS --- */
        ~MetalResource() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        MetalResource() noexcept = default;

    };

}
