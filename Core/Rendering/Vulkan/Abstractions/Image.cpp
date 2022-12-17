//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Image.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- SETTER METHODS --- */

    void Image::CreateImageView(const ImageAspectFlags givenAspectFlags)
    {
        // Check if an image view has already been generated
        if (imageViewCreated)
        {
            ASSERT_WARNING("Trying to create an image view for an image with an already existing view. Process automatically suspended");
            return;
        }

        // Set up image view creation info
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vkImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = (VkFormat) format;
        imageViewCreateInfo.subresourceRange.aspectMask = (VkImageAspectFlagBits) givenAspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        // Create the image view
        VK_ASSERT(
            vkCreateImageView(VulkanCore::GetLogicalDevice(), &imageViewCreateInfo, nullptr, &vkImageView),
            "Could not create image view for an image with dimensions of [" + std::to_string(dimensions.width) + ", " + std::to_string(dimensions.height) + ", " + std::to_string(dimensions.depth) + "], format [" + std::to_string((int) format) + "], [" +
            std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string((int) sampling) + "]"
        );

        imageViewCreated = true;
    }

    void Image::TransitionLayout(const ImageLayout newLayout)
    {
        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VulkanCore::GetDevice()->BeginSingleTimeCommands();

        // Create image memory barrier
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = (VkImageLayout) layout;								// Layout to transition from
        imageMemoryBarrier.newLayout = (VkImageLayout) newLayout;							// Layout to transition to
        imageMemoryBarrier.srcQueueFamilyIndex = ~0U;			            // Queue family to transition from
        imageMemoryBarrier.dstQueueFamilyIndex = ~0U;			            // Queue family to transition to
        imageMemoryBarrier.image = vkImage;									// Image being accessed and modified as part of barrier
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;				// First mip level to start alterations on
        imageMemoryBarrier.subresourceRange.levelCount = mipLevels;			// Number of mip levels to alter starting from baseMipLevel
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;				// First layer to start alterations on
        imageMemoryBarrier.subresourceRange.layerCount = 1;					// Number of layers to alter starting from baseArrayLayer

        // If transitioning from a depth image...
        if (newLayout == ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
        {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (format == ImageFormat::D32_SFLOAT_S8_UINT || format == ImageFormat::D24_UNORM_S8_UINT)
            {
                imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags srcStage = 0;
        VkPipelineStageFlags dstStage = 0;

        // If transitioning from a new or undefined image to an image that is ready to receive data...
        if (layout == ImageLayout::UNDEFINED && newLayout == ImageLayout::TRANSFER_DST_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  // The stage the transition must occur after
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;     // The stage the transition must occur before
        }
        // If transitioning from transfer destination to shader readable...
        else if (layout == ImageLayout::TRANSFER_DST_OPTIMAL && newLayout == ImageLayout::SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        // If transitioning from an undefined layout to one optimal for depth stencil...
        else if (layout == ImageLayout::UNDEFINED && newLayout == ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            ASSERT_ERROR("Transitioning images from [" + std::to_string((int) layout) + "] to [" + std::to_string((int) newLayout) + "] is not supported");
        }

        // Bind the pipeline barrier
        vkCmdPipelineBarrier(
            commandBuffer,
            srcStage, dstStage,		            // Pipeline stages (match to src and dst AccessMasks)
            0,						                        // Dependency flagsInfo
            0, nullptr,				        // Memory Barrier count + data
            0, nullptr,			// Buffer Memory Barrier count + data
            1, &imageMemoryBarrier	// Image Memory Barrier count + data
        );

        // End command buffer
        VulkanCore::GetDevice()->EndSingleTimeCommands(commandBuffer);

        // Change the current layout
        this->layout = newLayout;
    }

    void Image::DestroyImageView()
    {
        vkDestroyImageView(VulkanCore::GetLogicalDevice(), this->vkImageView, nullptr);
        imageViewCreated = false;
    }

    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
        : dimensions(createInfo.dimensions), mipLevels(createInfo.mipLevels), sampling(createInfo.sampling), format(createInfo.format)
    {
        // Set up image creation info
        VkImageCreateInfo vkImageCreateInfo{};
        vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;

        vkImageCreateInfo.extent.width = static_cast<uint32_t>(dimensions.width);
        vkImageCreateInfo.extent.height = static_cast<uint32_t>(dimensions.height);
        vkImageCreateInfo.extent.depth = static_cast<uint32_t>(dimensions.depth);

        vkImageCreateInfo.mipLevels = mipLevels;
        vkImageCreateInfo.arrayLayers = 1;
        vkImageCreateInfo.format = (VkFormat) format;
        vkImageCreateInfo.tiling = (VkImageTiling) createInfo.imageTiling;
        vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkImageCreateInfo.usage = (VkImageUsageFlagBits) createInfo.usageFlags;
        vkImageCreateInfo.samples = (VkSampleCountFlagBits) sampling;
        vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Create the Vulkan image
        VK_ASSERT(
            vkCreateImage(VulkanCore::GetLogicalDevice(), &vkImageCreateInfo, nullptr, &vkImage),
            "Failed to create image with dimensions of [" + std::to_string(dimensions.width) + ", " + std::to_string(dimensions.height) + ", " + std::to_string(dimensions.depth) + "], format [" + std::to_string((int) format) + "], [" +
            std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string((int) sampling) + "]"
        );

        // Retrieve its memory requirements
        VkMemoryRequirements imageMemoryRequirements;
        vkGetImageMemoryRequirements(VulkanCore::GetLogicalDevice(), vkImage, &imageMemoryRequirements);

        // Set up image memory allocation info
        VkMemoryAllocateInfo imageMemoryAllocateInfo{};
        imageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        imageMemoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
        imageMemoryAllocateInfo.memoryTypeIndex = VulkanCore::GetDevice()->FindMemoryTypeIndex(imageMemoryRequirements.memoryTypeBits, createInfo.memoryFlags);

        // Allocate the image to memory
        VK_ASSERT(
                vkAllocateMemory(VulkanCore::GetLogicalDevice(), &imageMemoryAllocateInfo, nullptr, &vkImageMemory),
                "Failed to allocate memory for image with dimensions of [" + std::to_string(dimensions.width) + ", " + std::to_string(dimensions.height) + ", " + std::to_string(dimensions.depth) + "], format [" + std::to_string((int) format) + "], [" +
                std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string((int) sampling) + "]"
        );

        // Bind the image to its corresponding memory
        vkBindImageMemory(VulkanCore::GetLogicalDevice(), vkImage, vkImageMemory, 0);
    }

    std::unique_ptr<Image> Image::Create(ImageCreateInfo imageCreateInfo)
    {
        return std::make_unique<Image>(imageCreateInfo);
    }

    std::unique_ptr<Image> Image::CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo)
    {
        return std::make_unique<Image>(swapchainImageCreateInfo);
    }

    Image::Image(const SwapchainImageCreateInfo &createInfo)
            : vkImage(createInfo.image), format(createInfo.format), sampling(createInfo.sampling), dimensions(createInfo.dimensions), layout(ImageLayout::PRESENT_SRC), swapchainImage(true)
    {

    }

    /* --- DESTRUCTOR --- */

    void Image::Destroy()
    {
        if (vkImage == VK_NULL_HANDLE) return;

        if (!swapchainImage)
        {
            vkDestroyImage(VulkanCore::GetLogicalDevice(), this->vkImage, nullptr);
            vkFreeMemory(VulkanCore::GetLogicalDevice(), this->vkImageMemory, nullptr);
        }
        if (imageViewCreated) vkDestroyImageView(VulkanCore::GetLogicalDevice(), this->vkImageView, nullptr);

        vkImage = VK_NULL_HANDLE;
    }
}