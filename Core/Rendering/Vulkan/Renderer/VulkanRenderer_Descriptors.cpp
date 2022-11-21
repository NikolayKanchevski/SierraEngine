//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateDescriptorSetLayout()
    {
        // Create the descriptor set layout
        auto descriptorSetLayoutBuilder = DescriptorSetLayout::Builder()
            .SetShaderStages(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBinding(UNIFORM_BUFFER_BINDING, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .AddBinding(STORAGE_BUFFER_BINDING, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            descriptorSetLayoutBuilder.AddBinding(BINDLESS_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, MAX_TEXTURES);
        }
        else
        {
            descriptorSetLayoutBuilder.AddBinding(DIFFUSE_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetLayoutBuilder.AddBinding(SPECULAR_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetLayoutBuilder.AddBinding(HEIGHT_MAP_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }

        descriptorSetLayout = descriptorSetLayoutBuilder.Build();
    }

    void VulkanRenderer::CreateDescriptorPool()
    {
        // Calculate the total descriptor count
        const uint32_t DESCRIPTOR_COUNT = 32768;

        // Create the descriptor pool
        descriptorPool = DescriptorPool::Builder()
            .SetMaxSets(DESCRIPTOR_COUNT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        .Build(descriptorSetLayout);

        // Save the descriptor pool to VulkanCore
        VulkanCore::SetDescriptorPool(descriptorPool);
    }

    void VulkanRenderer::CreateShaderBuffersDescriptorSets()
    {
        // Resize the uniform buffers array and write to each descriptor
        bufferDescriptorSets.resize(MAX_CONCURRENT_FRAMES);

        for (uint32_t i = MAX_CONCURRENT_FRAMES; i--;)
        {
            bufferDescriptorSets[i] = DescriptorSet::Build(descriptorPool);
            bufferDescriptorSets[i]->WriteBuffer(UNIFORM_BUFFER_BINDING, GET_UNIFORM_BUFFER(i));
            bufferDescriptorSets[i]->WriteBuffer(STORAGE_BUFFER_BINDING, GET_STORAGE_BUFFER(i));
            bufferDescriptorSets[i]->Allocate();
        }
    }

}