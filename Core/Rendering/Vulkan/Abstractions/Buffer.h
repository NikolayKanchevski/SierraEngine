//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct BufferCreateInfo
    {
        uint64 memorySize = 0;
        MemoryFlags memoryFlags = MemoryFlags::NONE;
        BufferUsage bufferUsage = BufferUsage::UNDEFINED;
    };

    /// @brief An abstraction class to make managing Vulkan buffers easier.
    class Buffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Buffer(const BufferCreateInfo &createInfo);
        [[nodiscard]] static UniquePtr<Buffer> Create(BufferCreateInfo bufferCreateInfo);
        [[nodiscard]] static SharedPtr<Buffer> CreateShared(BufferCreateInfo bufferCreateInfo);

        /* --- SETTER METHODS --- */
        void CopyFromPointer(void* pointer);
        void CopyImage(const Image& givenImage);
        void CopyToBuffer(const Buffer *otherBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const
        { return this->vkBuffer; }

        [[nodiscard]] inline VmaAllocation GetMemory() const
        { return this->vmaBufferAllocation; }

        [[nodiscard]] inline MemoryFlags GetMemoryFlags() const
        { return this->memoryFlags; }

        [[nodiscard]] inline uint64 GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline BufferUsage GetBufferUsage() const
        { return this->bufferUsage; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Buffer);

    private:
        VkBuffer vkBuffer;
        VmaAllocation vmaBufferAllocation;

        uint64 memorySize;
        MemoryFlags memoryFlags;
        BufferUsage bufferUsage;
    };

}