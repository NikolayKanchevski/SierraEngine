//
// Created by Nikolay Kanchevski on 11.01.24.
//

#pragma once

#include "../../PipelineLayout.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    struct MetalPipelineBinding
    {
        uint32 index = 0;
        union {
            struct {
                uint32 samplerIndex = 0;
            } textureData;
        } data = { };
    };

    class SIERRA_API MetalPipelineLayout final : public PipelineLayout, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalPipelineLayout(const MetalDevice &metalDevice, const PipelineLayoutCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MetalPipelineBinding GetBindingData(const uint32 binding) const { return bindings[binding]; };
        [[nodiscard]] inline MetalPipelineBinding GetPushConstantBinding() const { return bindings.back(); };
        [[nodiscard]] inline uint16 GetPushConstantSize() const { return pushConstantSize; }

        /* --- CONSTANTS --- */
        constexpr static NSUInteger VERTEX_BUFFER_SHADER_INDEX = 30;

        /* --- DESTRUCTOR --- */
        ~MetalPipelineLayout() override = default;

    private:
        std::vector<MetalPipelineBinding> bindings;
        uint16 pushConstantSize = 0;

    };

}