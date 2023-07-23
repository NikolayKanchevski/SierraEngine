//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"
#include "../../../Engine/Classes/MemoryObject.h"

namespace Sierra::Rendering
{
    struct BufferCreateInfo
    {
        uint64 memorySize;
        BufferUsage bufferUsage;
    };

    /// @brief An abstraction class to make managing Vulkan buffers easier.
    class Buffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Buffer(const BufferCreateInfo &createInfo);
        [[nodiscard]] static UniquePtr<Buffer> Create(const BufferCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        template<typename T>
        inline T* GetDataAs()
        {
            return reinterpret_cast<T*>(data.GetData());
        }

        /* --- SETTER METHODS --- */
        void Flush();
        void CopyFromPointer(const void* pointer, uint64 size = 0, uint64 offset = 0);
        void CopyToImage(const UniquePtr<Image> &givenImage);
        void CopyToBuffer(const UniquePtr<Buffer> &otherBuffer, uint64 size = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetMemorySize() const { return data.GetMemorySize(); }
        [[nodiscard]] inline Engine::MemoryObject& GetMemory() { return data; }

        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const { return vkBuffer; }
        [[nodiscard]] inline VmaAllocation GetMemoryAllocation() const { return vmaBufferAllocation; }
        [[nodiscard]] inline BufferUsage GetBufferUsage() const { return bufferUsage; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Buffer);

    private:
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VmaAllocation vmaBufferAllocation = nullptr;

        BufferUsage bufferUsage;
        Engine::MemoryObject data;
    };

}