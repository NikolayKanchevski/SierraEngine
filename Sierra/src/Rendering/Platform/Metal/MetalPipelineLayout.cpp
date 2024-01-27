//
// Created by Nikolay Kanchevski on 11.01.24.
//

#include "MetalPipelineLayout.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalPipelineLayout::MetalPipelineLayout(const MetalDevice &metalDevice, const PipelineLayoutCreateInfo &createInfo)
        : PipelineLayout(createInfo), MetalResource(createInfo.name), pushConstantSize(createInfo.pushConstantSize)
    {
        // Calculate binding index offsets (indexing on Metal is not supported, so a separate resource for every entry in the array is created, and its index is "global")
        uint32 currentIndex = 0;
        bindingIndices.resize(createInfo.bindings.size());
        for (uint32 i = 0; i < createInfo.bindings.size(); i++)
        {
            bindingIndices[i] = currentIndex;
            currentIndex += (createInfo.bindings.begin() + i)->arraySize;
        }

        // Push constant buffer is always the last indexed buffer
        pushConstantIndex = currentIndex;
    }

}
