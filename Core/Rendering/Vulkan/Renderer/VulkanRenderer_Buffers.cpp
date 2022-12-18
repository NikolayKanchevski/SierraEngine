//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{
    void VulkanRenderer::CreateShaderBuffers()
    {
        // Resize uniform buffers array
        shaderBuffers.reserve(maxConcurrentFrames * SHADER_BUFFERS_COUNT);

        const uint64_t uniformDataSize = sizeof(UniformData);
        const uint64_t storageDataSize = sizeof(StorageData);

        // Create uniform buffers
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            shaderBuffers.push_back(Buffer::Create({
                .memorySize = uniformDataSize,
                .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
                .bufferUsage = UNIFORM_BUFFER
            }));
        }

        // Create storage buffers
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            shaderBuffers.push_back(Buffer::Create({
                .memorySize = storageDataSize,
                .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
                .bufferUsage = STORAGE_BUFFER
            }));
        }
    }

    void VulkanRenderer::UpdateShaderBuffers(uint32_t imageIndex)
    {
        GET_UNIFORM_BUFFER(imageIndex)->CopyFromPointer(VulkanCore::GetUniformDataPtr());
        GET_STORAGE_BUFFER(imageIndex)->CopyFromPointer(VulkanCore::GetStorageDataPtr());
    }

}

