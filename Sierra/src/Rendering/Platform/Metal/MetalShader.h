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

#include "../../Shader.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalShader final : public Shader, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalShader(const MetalDevice &device, const ShaderCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] id<MTLFunction> GetEntryFunction() const { return entryFunction; }

        /* --- DESTRUCTOR --- */
        ~MetalShader() override;

    private:
        id<MTLFunction> entryFunction = nil;

    };

}
