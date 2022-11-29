//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include <cstdint>
#include "Image.h"
#include "../VulkanCore.h"
#include "../VulkanUtilities.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- SETTER METHODS --- */

    void Image::CreateImageView(const VkImageAspectFlags givenAspectFlags)
    {
        // Check if an image view has already been generated
        if (imageViewGenerated)
        {
            ASSERT_WARNING("Trying to create an image view for an image with an already existing view. Process automatically suspended");
            return;
        }

        // Set up image view creation info
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vkImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.subresourceRange.aspectMask = givenAspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        // Create the image view
        VK_ASSERT(
                vkCreateImageView(VulkanCore::GetLogicalDevice(), &imageViewCreateInfo, nullptr, &vkImageView),
                "Could not create image view for an image with dimensions of [" + std::to_string(dimensions.x) + ", " + std::to_string(dimensions.y) + ", " + std::to_string(dimensions.z) + "], format [" + std::to_string(format) + "], [" +
                std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string(sampling) + "]"
        );

        imageViewGenerated = true;
    }

    void Image::TransitionLayout(const VkImageLayout newLayout)
    {
        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VulkanUtilities::BeginSingleTimeCommands();

        // Create image memory barrier
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = layout;								// Layout to transition from
        imageMemoryBarrier.newLayout = newLayout;							// Layout to transition to
        imageMemoryBarrier.srcQueueFamilyIndex = ~0U;			            // Queue family to transition from
        imageMemoryBarrier.dstQueueFamilyIndex = ~0U;			            // Queue family to transition to
        imageMemoryBarrier.image = vkImage;									// Image being accessed and modified as part of barrier
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;				// First mip level to start alterations on
        imageMemoryBarrier.subresourceRange.levelCount = mipLevels;			// Number of mip levels to alter starting from baseMipLevel
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;				// First layer to start alterations on
        imageMemoryBarrier.subresourceRange.layerCount = 1;					// Number of layers to alter starting from baseArrayLayer

        // If transitioning from a depth image...
        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
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
        if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  // The stage the transition must occur after
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;     // The stage the transition must occur before
        }


//        else if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//        {
//            imageMemoryBarrier.srcAccessMask = 0;
//            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//
//            VkImageMemoryBarrier2KHR imageMemoryBarrier2 = {
//                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//                    .oldLayout = layout,
//                    .newLayout = newLayout,
//                    .srcQueueFamilyIndex = ~0U,
//                    .dstQueueFamilyIndex = ~0U,
//                    .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
//                    .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
//                    .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
//                    .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT_KHR,
//                    .oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
//                    .newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
//                    .image = vkImage,
//                    .subresourceRange.baseMipLevel = 0,
//                    .subresourceRange.levelCount = mipLevels,
//                    .subresourceRange.baseArrayLayer = 0,
//                    .subresourceRange.layerCount = 1
//            };
//
//            VkDependencyInfoKHR dependencyInfo = {
//                    ...
//                    1,                      // imageMemoryBarrierCount
//                    &imageMemoryBarrier,    // pImageMemoryBarriers
//                    ...
//            }
//
//            vkCmdPipelineBarrier2KHR(commandBuffer, &dependencyInfo);
//        }



        // If transitioning from transfer destination to shader readable...
        else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        // If transitioning from an undefined layout to one optimal for depth stencil...
        else if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            ASSERT_ERROR("Transitioning images from [" + std::to_string(layout) + "] to [" + std::to_string(newLayout) + "] is not supported");
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
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);

        // Change the current layout
        this->layout = newLayout;
    }

    void Image::DestroyImageView()
    {
        vkDestroyImageView(VulkanCore::GetLogicalDevice(), this->vkImageView, nullptr);
        imageViewGenerated = false;
    }

    /* --- CONSTRUCTORS --- */

    Image::Image(ImageCreateInfo imageCreateInfo)
        : dimensions(imageCreateInfo.dimensions), mipLevels(imageCreateInfo.mipLevels), sampling(imageCreateInfo.sampling), format(imageCreateInfo.format)
    {
        // Set up image creation info
        VkImageCreateInfo vkImageCreateInfo{};
        vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;

        vkImageCreateInfo.extent.width = static_cast<uint32_t>(dimensions.x);
        vkImageCreateInfo.extent.height = static_cast<uint32_t>(dimensions.y);
        vkImageCreateInfo.extent.depth = static_cast<uint32_t>(dimensions.z);

        vkImageCreateInfo.mipLevels = mipLevels;
        vkImageCreateInfo.arrayLayers = 1;
        vkImageCreateInfo.format = format;
        vkImageCreateInfo.tiling = imageCreateInfo.imageTiling;
        vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkImageCreateInfo.usage = imageCreateInfo.usageFlags;
        vkImageCreateInfo.samples = sampling;
        vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Create the Vulkan image
        VK_ASSERT(
                vkCreateImage(VulkanCore::GetLogicalDevice(), &vkImageCreateInfo, nullptr, &vkImage),
                "Failed to create image with dimensions of [" + std::to_string(dimensions.x) + ", " + std::to_string(dimensions.y) + ", " + std::to_string(dimensions.z) + "], format [" + std::to_string(format) + "], [" +
                std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string(sampling) + "]"
        );

        // Retrieve its memory requirements
        VkMemoryRequirements imageMemoryRequirements;
        vkGetImageMemoryRequirements(VulkanCore::GetLogicalDevice(), vkImage, &imageMemoryRequirements);

        // Set up image memory allocation info
        VkMemoryAllocateInfo imageMemoryAllocateInfo{};
        imageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        imageMemoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
        imageMemoryAllocateInfo.memoryTypeIndex = VulkanUtilities::FindMemoryTypeIndex(imageMemoryRequirements.memoryTypeBits, imageCreateInfo.memoryFlags);

        // Allocate the image to memory
        VK_ASSERT(
                vkAllocateMemory(VulkanCore::GetLogicalDevice(), &imageMemoryAllocateInfo, nullptr, &vkImageMemory),
                "Failed to allocate memory for image with dimensions of [" + std::to_string(dimensions.x) + ", " + std::to_string(dimensions.y) + ", " + std::to_string(dimensions.z) + "], format [" + std::to_string(format) + "], [" +
                std::to_string(mipLevels) + "] mip levels, and sampling of [" + std::to_string(sampling) + "]"
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

    Image::Image(const SwapchainImageCreateInfo swapchainImageCreateInfo)
            : vkImage(swapchainImageCreateInfo.image), format(swapchainImageCreateInfo.format), sampling(swapchainImageCreateInfo.sampling), dimensions(swapchainImageCreateInfo.dimensions), layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR), swapchainImage(true)
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
        if (imageViewGenerated) vkDestroyImageView(VulkanCore::GetLogicalDevice(), this->vkImageView, nullptr);

        vkImage = VK_NULL_HANDLE;
    }
}