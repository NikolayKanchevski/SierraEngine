//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Buffer.h"

#include "../VK.h"


namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    Buffer::Buffer(const BufferCreateInfo &createInfo)
        : data(Engine::MemoryObject(createInfo.memorySize, 0)), bufferUsage(createInfo.bufferUsage)
    {
        // Set up buffer creation info
        VkBufferCreateInfo vkBufferCreateInfo{};
        vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfo.size = data.GetMemorySize();
        vkBufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(createInfo.bufferUsage);
        vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Set up buffer allocation info
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationCreateInfo.memoryTypeBits = 0;
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate buffer
        VK_ASSERT(
            vmaCreateBuffer(VK::GetMemoryAllocator(), &vkBufferCreateInfo, &allocationCreateInfo, &vkBuffer, &vmaBufferAllocation, nullptr),
            FORMAT_STRING("Failed to create buffer with size of [{0}] for [{1}] usage", data.GetMemorySize(), VK_TO_STRING(bufferUsage, BufferUsageFlagBits))
        );

        // Map memory
        vmaMapMemory(VK::GetMemoryAllocator(), vmaBufferAllocation, &data.GetData());
        memset(data.GetData(), 0, data.GetMemorySize());
    }

    UniquePtr<Buffer> Buffer::Create(const BufferCreateInfo &createInfo)
    {
        return std::make_unique<Buffer>(createInfo);
    }

    /* --- SETTER METHODS --- */

    void Buffer::Flush()
    {
        vmaFlushAllocation(VK::GetMemoryAllocator(), vmaBufferAllocation, 0, data.GetMemorySize());
    }

    void Buffer::CopyFromPointer(const void* pointer, const uint64 size, const uint64 offset)
    {
        // Copy memory data to Vulkan buffer
        data.SetDataByOffset(pointer, size != 0 ? size : data.GetMemorySize(), offset);
        Flush();
    }

    void Buffer::CopyToImage(const UniquePtr<Image> &givenImage)
    {
        // Create a temporary command buffer
        auto commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

        // Set up image copy region
        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = static_cast<VkImageAspectFlags>(givenImage->GetAspectFlags());
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = givenImage->GetLayerCount();
        copyRegion.imageOffset.x = 0;
        copyRegion.imageOffset.y = 0;
        copyRegion.imageOffset.z = 0;
        copyRegion.imageExtent.width = givenImage->GetWidth();
        copyRegion.imageExtent.height = givenImage->GetHeight();
        copyRegion.imageExtent.depth = givenImage->GetDepth();

        // Copy the image to the buffer
        vkCmdCopyBufferToImage(commandBuffer->GetVulkanCommandBuffer(), vkBuffer, givenImage->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        // Destroy the temporary command buffer
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
    }

    void Buffer::CopyToBuffer(const UniquePtr<Buffer> &otherBuffer, const uint64 size, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        // Create a temporary command buffer
        auto commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

        // Set up the buffer's copy region
        VkBufferCopy copyRegion{};
        copyRegion.size = size != 0 ? size : data.GetMemorySize();
        copyRegion.srcOffset = sourceOffset;
        copyRegion.dstOffset = destinationOffset;

        // Copy the buffer
        vkCmdCopyBuffer(commandBuffer->GetVulkanCommandBuffer(), vkBuffer, otherBuffer->vkBuffer, 1, &copyRegion);

        // Destroy the temporary command buffer
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    void Buffer::Destroy()
    {
        // Unmap the memory
        vmaUnmapMemory(VK::GetMemoryAllocator(), vmaBufferAllocation);
        vmaDestroyBuffer(VK::GetMemoryAllocator(), vkBuffer, vmaBufferAllocation);
    }
}