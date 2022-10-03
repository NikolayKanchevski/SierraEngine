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
    /// \brief An abstraction class to make managing Vulkan buffers easier.
    class Buffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Buffer(unsigned long givenMemorySize, VkMemoryPropertyFlags givenMemoryFlags, VkBufferUsageFlags givenBufferUsage);

        class Builder
        {
        public:
            Builder& SetMemorySize(unsigned long givenMemorySize);
            Builder& SetMemoryFlags(VkMemoryPropertyFlags givenMemoryFlags);
            Builder& SetUsageFlags(VkBufferUsageFlags givenBufferUsage);

            [[nodiscard]] std::unique_ptr<Buffer> Build() const;

        private:
            unsigned long memorySize;
            VkMemoryPropertyFlags memoryFlags;
            VkBufferUsageFlags bufferUsage;
        };

        /* --- SETTER METHODS --- */
        void CopyFromPointer(void* pointer, unsigned long offset = 0);
        void CopyImage(const Image& givenImage, glm::vec3 imageOffset = { 0, 0, 0 }, unsigned long offset = 0);
        void CopyToBuffer(const Buffer& otherBuffer);

        void DestroyBuffer();
        void FreeMemory();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const
        { return this->vkBuffer; }

        [[nodiscard]] inline VkDeviceMemory GetVulkanMemory() const
        { return this->vkBufferMemory; }

        [[nodiscard]] inline VkMemoryPropertyFlags GetMemoryFlags() const
        { return this->memoryFlags; }

        [[nodiscard]] inline unsigned long GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline VkBufferUsageFlags GetBufferUsage() const
        { return this->bufferUsage; }

        /* --- DESTRUCTOR --- */
        ~Buffer();
        Buffer(const Buffer &) = delete;
        Buffer &operator=(const Buffer &) = delete;

    private:

        VkBuffer vkBuffer;
        VkDeviceMemory vkBufferMemory;

        unsigned long memorySize;
        VkMemoryPropertyFlags memoryFlags;
        VkBufferUsageFlags bufferUsage;
    };

}