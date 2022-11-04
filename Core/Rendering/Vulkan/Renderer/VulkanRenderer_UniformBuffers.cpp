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
        uniformBuffers.resize(swapchainImages.size());

        // Create uniform buffers
        for (size_t i = 0; i < swapchainImages.size(); i++)
        {
            uniformBuffers[i] = Buffer::Create({
               .memorySize = uniformDataSize,
               .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               .bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            });
        }
    }

    void VulkanRenderer::UpdateUniformBuffers(const uint32_t imageIndex)
    {
        uniformBuffers[imageIndex]->CopyFromPointer(&uniformData);
    }

}

