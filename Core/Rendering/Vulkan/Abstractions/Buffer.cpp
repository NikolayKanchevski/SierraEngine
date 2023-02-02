//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Buffer.h"

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    /* --- CONSTRUCTORS --- */

    Buffer::Buffer(const BufferCreateInfo &createInfo)
        : memorySize(createInfo.memorySize), memoryFlags(createInfo.memoryFlags), bufferUsage(createInfo.bufferUsage) {
        // Set up buffer creation info
        VkBufferCreateInfo vkBufferCreateInfo{};
        vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfo.size = memorySize;
        vkBufferCreateInfo.usage = (VkBufferUsageFlags) bufferUsage;
        vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Set up buffer allocation info
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationCreateInfo.memoryTypeBits = std::numeric_limits<uint>::max();
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate buffer
        VK_ASSERT(
            vmaCreateBuffer(VK::GetMemoryAllocator(), &vkBufferCreateInfo, &allocationCreateInfo, &vkBuffer, &vmaBufferAllocation, nullptr),
            fmt::format("Failed to create buffer with size of [{0}] for [{1}] usage", memorySize, (int) bufferUsage)
        );
    }

    UniquePtr<Buffer> Buffer::Create(const BufferCreateInfo bufferCreateInfo)
    {
        return std::make_unique<Buffer>(bufferCreateInfo);
    }

    SharedPtr<Buffer> Buffer::CreateShared(const BufferCreateInfo bufferCreateInfo)
    {
        return std::make_shared<Buffer>(bufferCreateInfo);
    }

    /* --- SETTER METHODS --- */

    void Buffer::CopyFromPointer(void *pointer)
    {
        // Create an empty pointer
        void *data;

        // Map memory
        vmaMapMemory(VK::GetMemoryAllocator(), vmaBufferAllocation, &data);

        // Copy memory data to Vulkan buffer
        memcpy(data, pointer, memorySize);

        // Unmap the memory
        vmaUnmapMemory(VK::GetMemoryAllocator(), vmaBufferAllocation);
    }

    void Buffer::CopyImage(const Image& givenImage)
    {
        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

        // Set up image copy region
        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = givenImage.GetLayerCount();
        copyRegion.imageOffset.x = 0;
        copyRegion.imageOffset.y = 0;
        copyRegion.imageOffset.z = 0;
        copyRegion.imageExtent.width = static_cast<uint>(givenImage.GetWidth());
        copyRegion.imageExtent.height = static_cast<uint>(givenImage.GetHeight());
        copyRegion.imageExtent.depth = static_cast<uint>(givenImage.GetDepth());

        // Copy the image to the buffer
        vkCmdCopyBufferToImage(commandBuffer, this->vkBuffer, givenImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        // Destroy the temporary command buffer
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
    }

    void Buffer::CopyToBuffer(const Buffer *otherBuffer)
    {
        // Check if the two buffers are compatible
        ASSERT_ERROR_IF(this->memorySize != otherBuffer->memorySize, "Cannot copy data from one buffer to another with a different memory size!");

        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

        // Set up the buffer's copy region
        VkBufferCopy copyRegion{};
        copyRegion.size = this->memorySize;

        // Copy the buffer
        vkCmdCopyBuffer(commandBuffer, vkBuffer, otherBuffer->vkBuffer, 1, &copyRegion);

        // Destroy the temporary command buffer
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    void Buffer::Destroy()
    {
        vmaDestroyBuffer(VK::GetMemoryAllocator(), vkBuffer, vmaBufferAllocation);
    }
}