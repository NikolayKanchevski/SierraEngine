//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

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
        [[nodiscard]] inline MTL::Function* GetEntryFunction() const { return entryFunction; }

        /* --- DESTRUCTOR --- */
        ~MetalShader() override;

    private:
        MTL::Library* library = nullptr;
        MTL::Function* entryFunction = nullptr;

    };

}
