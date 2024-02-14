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
        uint32 currentBufferIndex = 0;
        uint32 currentTextureIndex = 0;
        uint32 currentSamplerIndex = 0;

        // Calculate binding index offsets (indexing on Metal is not supported, so a separate resource for every entry in the array is created)
        bindings.resize(createInfo.bindings.size() + static_cast<uint32>(createInfo.pushConstantSize > 0));
        for (uint32 i = 0; i < createInfo.bindings.size(); i++)
        {
            const PipelineBinding &binding = *(createInfo.bindings.begin() + i);
            switch (binding.type)
            {
                case PipelineBindingType::Undefined:
                {
                    SR_ERROR("[Metal]: Cannot create pipeline layout [{0}], as binding [{1}] must not be of type [PipelineBindingType::Undefined]!", GetName(), i);
                    break;
                }
                case PipelineBindingType::UniformBuffer:
                case PipelineBindingType::StorageBuffer:
                {
                    bindings[i] = { .index = currentBufferIndex };
                    currentBufferIndex += binding.arraySize;
                    break;
                }
                case PipelineBindingType::Image:
                {
                    bindings[i] = { .index = currentTextureIndex };
                    currentTextureIndex += binding.arraySize;
                    break;
                }
                case PipelineBindingType::Texture:
                case PipelineBindingType::InputAttachment:
                {
                    bindings[i] = { .index = currentTextureIndex, .data = { .textureData = { .samplerIndex = currentSamplerIndex } }};
                    currentTextureIndex += binding.arraySize;
                    currentSamplerIndex += binding.arraySize;
                    break;
                }
            }

        }

        // Push constants range is always the last indexed buffer
        if (createInfo.pushConstantSize > 0) bindings.back().index = currentBufferIndex;
    }

}
