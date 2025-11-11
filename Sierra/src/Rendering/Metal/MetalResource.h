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
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] RenderingBackendType GetBackendType() const noexcept override { return RenderingBackendType::Metal; }

        /* --- COPY SEMANTICS --- */
        MetalResource(const MetalResource&) = delete;
        MetalResource& operator=(const MetalResource&) = delete;

        /* --- DESTRUCTORS --- */
        ~MetalResource() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit MetalResource(std::string_view name);

        /* --- MOVE SEMANTICS --- */
        MetalResource(MetalResource&&) noexcept = default;
        MetalResource& operator=(MetalResource&&) noexcept = default;

    private:
        std::string name = { };

    };

}
