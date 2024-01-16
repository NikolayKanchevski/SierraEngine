//
// Created by Nikolay Kanchevski on 11.01.24.
//

#pragma once

#include "../../PipelineLayout.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalPipelineLayout : public PipelineLayout, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalPipelineLayout(const MetalDevice &metalDevice, const PipelineLayoutCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetBindingIndex(const uint32 binding, const uint32 arrayIndex = 0) const { return bindingIndices[arrayIndex]; };
        [[nodiscard]] inline uint32 GetPushConstantIndex() const { return pushConstantIndex; };
        [[nodiscard]] inline uint16 GetPushConstantSize() const { return pushConstantSize; }

        /* --- DESTRUCTOR --- */
        ~MetalPipelineLayout() override = default;

    private:
        uint16 pushConstantSize = 0;
        std::vector<uint32> bindingIndices;
        uint32 pushConstantIndex = 0;

    };

}