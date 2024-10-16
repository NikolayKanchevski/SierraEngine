//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLFunction = void;
    }
#endif

#include "../Shader.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalShader final : public Shader, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalShader(const MetalDevice& device, const ShaderCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override;
        [[nodiscard]] id<MTLFunction> GetEntryFunction() const noexcept { return entryFunction; }

        /* --- COPY SEMANTICS --- */
        MetalShader(const MetalShader&) = delete;
        MetalShader& operator=(const MetalShader&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalShader(MetalShader&&) = delete;
        MetalShader& operator=(MetalShader&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalShader() noexcept override;

    private:
        id<MTLFunction> entryFunction = nil;

    };

}
