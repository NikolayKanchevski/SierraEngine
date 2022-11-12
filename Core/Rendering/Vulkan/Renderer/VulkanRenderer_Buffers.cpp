//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{
    void VulkanRenderer::CreateShaderBuffers()
    {
        // Create uniform arrays
//        uniformData.directionalLights.resize(VulkanCore::MAX_DIRECTIONAL_LIGHTS);
//        uniformData.pointLights.resize(VulkanCore::MAX_POINT_LIGHTS);
//        uniformData.spotLights.resize(VulkanCore::MAX_SPOTLIGHT_LIGHTS);

        // Resize uniform buffers array
        shaderBuffers.reserve(MAX_CONCURRENT_FRAMES * SHADER_BUFFERS_COUNT);

        // Create uniform buffers
        for (uint32_t i = MAX_CONCURRENT_FRAMES; i--;)
        {
            shaderBuffers.push_back(Buffer::Create({
                .memorySize = uniformDataSize,
                .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                .bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            }));
        }

        // Create storage buffers
        for (uint32_t i = MAX_CONCURRENT_FRAMES; i--;)
        {
            shaderBuffers.push_back(Buffer::Create({
                .memorySize = storageDataSize,
                .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            }));
        }
    }

    void VulkanRenderer::UpdateShaderBuffers(uint32_t imageIndex)
    {
        GET_UNIFORM_BUFFER(imageIndex)->CopyFromPointer(VulkanCore::GetUniformDataPtr());
        GET_STORAGE_BUFFER(imageIndex)->CopyFromPointer(VulkanCore::GetStorageDataPtr());
    }

}

