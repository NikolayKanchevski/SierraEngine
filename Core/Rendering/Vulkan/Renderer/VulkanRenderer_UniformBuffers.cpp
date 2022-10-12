//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateUniformBuffers()
    {
        // Create uniform arrays
//        uniformData.directionalLights.resize(VulkanCore::MAX_DIRECTIONAL_LIGHTS);
//        uniformData.pointLights.resize(VulkanCore::MAX_POINT_LIGHTS);
//        uniformData.spotLights.resize(VulkanCore::MAX_SPOTLIGHT_LIGHTS);

        // Resize uniform buffers array
        uniformBuffers.resize(MAX_CONCURRENT_FRAMES);

        // For each concurrent frame
        for (int i = MAX_CONCURRENT_FRAMES; i--;)
        {
            uniformBuffers[i] = Buffer::Builder()
                .SetMemorySize(uniformDataSize)
                .SetMemoryFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                .SetUsageFlags(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
            .Build();
        }
    }

    void VulkanRenderer::UpdateUniformBuffers(const uint32_t imageIndex)
    {
        uniformBuffers[imageIndex]->CopyFromPointer(&uniformData);
//        printf((std::to_string(uniformData.view[0][1])))
    }

}

