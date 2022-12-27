//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Image.h"

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
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

        // Set up image allocation info
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationCreateInfo.memoryTypeBits = std::numeric_limits<uint32_t>::max();
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate image
        VK_ASSERT(
            vmaCreateImage(VK::GetMemoryAllocator(), &vkImageCreateInfo, &allocationCreateInfo, &vkImage, &vmaImageAllocation, nullptr),
            "Failed to allocate memory for image with dimensions of [" + std::to_string(dimensions.width) + ", " + std::to_string(dimensions.height) + ", " + std::to_string(dimensions.depth) + "], format [" + std::to_string((int) format) + "], [" +
            std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string((int) sampling) + "]"
        );
    }

    std::unique_ptr<Image> Image::Create(ImageCreateInfo imageCreateInfo)
    {
        return std::make_unique<Image>(imageCreateInfo);
    }

    Image::Image(const SwapchainImageCreateInfo &createInfo)
        : vkImage(createInfo.image), format(createInfo.format), sampling(createInfo.sampling), dimensions(createInfo.dimensions), layout(LAYOUT_PRESENT_SRC), swapchainImage(true)
    {

    }

    std::unique_ptr<Image> Image::CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo)
    {
        return std::make_unique<Image>(swapchainImageCreateInfo);
    }

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
            vkCreateImageView(VK::GetLogicalDevice(), &imageViewCreateInfo, nullptr, &vkImageView),
            "Could not create image view for an image with dimensions of [" + std::to_string(dimensions.width) + ", " + std::to_string(dimensions.height) + ", " + std::to_string(dimensions.depth) + "], format [" + std::to_string((int) format) + "], [" +
            std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string((int) sampling) + "]"
        );

        imageViewCreated = true;
    }

    void Image::TransitionLayout(const ImageLayout newLayout)
    {
        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

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
        if (newLayout == LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
        {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (format == FORMAT_D32_SFLOAT_S8_UINT || format == FORMAT_D24_UNORM_S8_UINT)
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
        if (layout == LAYOUT_UNDEFINED && newLayout == LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  // The stage the transition must occur after
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;     // The stage the transition must occur before
        }
        // If transitioning from transfer destination to shader readable...
        else if (layout == LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
            // If transitioning from an undefined layout to one optimal for depth stencil...
        else if (layout == LAYOUT_UNDEFINED && newLayout == LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
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
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);

        // Change the current layout
        this->layout = newLayout;
    }

    void Image::DestroyImageView()
    {
        vkDestroyImageView(VK::GetLogicalDevice(), this->vkImageView, nullptr);
        imageViewCreated = false;
    }

    /* --- DESTRUCTOR --- */

    void Image::Destroy()
    {
        if (vkImage == VK_NULL_HANDLE) return;

        if (!swapchainImage)
        {
            vmaDestroyImage(VK::GetMemoryAllocator(), vkImage, vmaImageAllocation);
        }

        if (imageViewCreated) vkDestroyImageView(VK::GetLogicalDevice(), this->vkImageView, nullptr);

        vkImage = VK_NULL_HANDLE;
    }
}