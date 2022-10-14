//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <glm/vec3.hpp>
#include <memory>
#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /// @brief An abstraction class to make managing Vulkan buffers easier.
    class Buffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Buffer(uint64_t givenMemorySize, VkMemoryPropertyFlags givenMemoryFlags, VkBufferUsageFlags givenBufferUsage);

        class Builder
        {
        public:
            Builder& SetMemorySize(uint64_t givenMemorySize);
            Builder& SetMemoryFlags(VkMemoryPropertyFlags givenMemoryFlags);
            Builder& SetUsageFlags(VkBufferUsageFlags givenBufferUsage);

            [[nodiscard]] std::unique_ptr<Buffer> Build() const;

        private:
            uint64_t memorySize;
            VkMemoryPropertyFlags memoryFlags;
            VkBufferUsageFlags bufferUsage;
        };

        /* --- SETTER METHODS --- */
        void CopyFromPointer(void* pointer, uint64_t offset = 0);
        void CopyImage(const Image& givenImage, glm::vec3 imageOffset = { 0, 0, 0 }, uint64_t offset = 0);
        void CopyToBuffer(const std::unique_ptr<Buffer> &otherBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const
        { return this->vkBuffer; }

        [[nodiscard]] inline VkDeviceMemory GetVulkanMemory() const
        { return this->vkBufferMemory; }

        [[nodiscard]] inline VkMemoryPropertyFlags GetMemoryFlags() const
        { return this->memoryFlags; }

        [[nodiscard]] inline uint64_t GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline VkBufferUsageFlags GetBufferUsage() const
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
        VkMemoryPropertyFlags memoryFlags;
        VkBufferUsageFlags bufferUsage;
    };

}