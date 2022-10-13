//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateDescriptorSetLayout()
    {
        // Create the descriptor set layout
        descriptorSetLayout = DescriptorSetLayout::Builder()
                .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
//                .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
//                .AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build();
    }

    void VulkanRenderer::CreateDescriptorPool()
    {
        // Calculate the total descriptor count
        const uint32_t DESCRIPTOR_COUNT = MAX_CONCURRENT_FRAMES + (MAX_TEXTURES * 2);

        // Create the descriptor pool
        descriptorPool = DescriptorPool::Builder()
            .SetMaxSets(DESCRIPTOR_COUNT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_COUNT)
//            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DESCRIPTOR_COUNT)
        .Build(descriptorSetLayout);
    }

    void VulkanRenderer::CreateUniformDescriptorSets()
    {
        // Create information on the buffer
        VkDescriptorBufferInfo uniformBufferInfo{};
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = uniformDataSize;

        // Resize the uniform buffers array and write to each descriptor
        uniformDescriptorSets.resize(MAX_CONCURRENT_FRAMES);

        for (int i = MAX_CONCURRENT_FRAMES; i--;)
        {
            uniformBufferInfo.buffer = uniformBuffers[i]->GetVulkanBuffer();

            uniformDescriptorSets[i] = DescriptorSet::Build(descriptorPool);
            uniformDescriptorSets[i]->WriteBuffer(0, &uniformBufferInfo);
            uniformDescriptorSets[i]->Allocate();
        }
    }

}