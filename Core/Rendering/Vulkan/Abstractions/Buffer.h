//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <cstdint>
#include <memory>
#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct BufferCreateInfo
    {
        uint64_t memorySize = 0;
        MemoryFlags memoryFlags = MemoryFlags::NONE;
        BufferUsage bufferUsage = BufferUsage::UNDEFINED;
    };

    /// @brief An abstraction class to make managing Vulkan buffers easier.
    class Buffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Buffer(const BufferCreateInfo &createInfo);
        [[nodiscard]] static std::unique_ptr<Buffer> Create(BufferCreateInfo bufferCreateInfo);
        [[nodiscard]] static std::shared_ptr<Buffer> CreateShared(BufferCreateInfo bufferCreateInfo);

        /* --- SETTER METHODS --- */
        void CopyFromPointer(void* pointer, uint64_t offset = 0);
        void CopyImage(const Image& givenImage, glm::vec3 imageOffset = { 0, 0, 0 }, uint64_t offset = 0);
        void CopyToBuffer(const Buffer *otherBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const
        { return this->vkBuffer; }

        [[nodiscard]] inline VkDeviceMemory GetVulkanMemory() const
        { return this->vkBufferMemory; }

        [[nodiscard]] inline MemoryFlags GetMemoryFlags() const
        { return this->memoryFlags; }

        [[nodiscard]] inline uint64_t GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline BufferUsage GetBufferUsage() const
        { return this->bufferUsage; }

        [[nodiscard]] inline uint64_t GetOffset() const { return 0; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Buffer(const Buffer &) = delete;
        Buffer &operator=(const Buffer &) = delete;

    private:
        VkBuffer vkBuffer;
        VkDeviceMemory vkBufferMemory;

        uint64_t memorySize;
        MemoryFlags memoryFlags;
        BufferUsage bufferUsage;
        bool destroyed = false;
    };

}