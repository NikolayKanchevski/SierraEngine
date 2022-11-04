//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateDescriptorSetLayout()
    {
        // Create the descriptor set layout
        auto descriptorSetLayoutBuilder = DescriptorSetLayout::Builder()
            .SetShaderStages(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            descriptorSetLayoutBuilder.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, MAX_TEXTURES);
        }
        else
        {
            descriptorSetLayoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetLayoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }

        descriptorSetLayout = descriptorSetLayoutBuilder.Build();
    }

    void VulkanRenderer::CreateDescriptorPool()
    {
        // Calculate the total descriptor count
        const uint32_t DESCRIPTOR_COUNT = MAX_CONCURRENT_FRAMES + (MAX_TEXTURES * 2);

        // Create the descriptor pool
        descriptorPool = DescriptorPool::Builder()
            .SetMaxSets(DESCRIPTOR_COUNT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_COUNT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DESCRIPTOR_COUNT)
        .Build(descriptorSetLayout);

        // Save the descriptor pool to VulkanCore
        VulkanCore::SetDescriptorPool(descriptorPool);
    }

    void VulkanRenderer::CreateUniformDescriptorSets()
    {
        // Resize the uniform buffers array and write to each descriptor
        uniformDescriptorSets.resize(MAX_CONCURRENT_FRAMES);

        for (uint32_t i = MAX_CONCURRENT_FRAMES; i--;)
        {
            uniformDescriptorSets[i] = DescriptorSet::Build(descriptorPool);
            uniformDescriptorSets[i]->WriteBuffer(0, uniformBuffers[i]);
            uniformDescriptorSets[i]->Allocate();
        }
    }

}