//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Image.h"
#include "../VulkanCore.h"
#include "../VulkanUtilities.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- SETTER METHODS --- */

    Image::Builder& Image::Builder::SetDimensions(const glm::vec3 givenDimensions)
    {
        // Save the given size locally
        this->dimensions = givenDimensions;
        return *this;
    }

    Image::Builder& Image::Builder::SetWidth(const uint32_t givenWidth)
    {
        // Save the given width locally
        this->dimensions.x = (float) givenWidth;
        return *this;
    }

    Image::Builder& Image::Builder::SetHeight(const uint32_t givenHeight)
    {
        // Save the given height locally
        this->dimensions.y = (float) givenHeight;
        return *this;
    }

    Image::Builder& Image::Builder::SetDepth(const uint32_t givenDepth)
    {
        // Save the given depth locally
        this->dimensions.z = (float) givenDepth;
        return *this;
    }

    Image::Builder& Image::Builder::SetMipLevels(const uint32_t givenMipLevels)
    {
        // Save the given mip levels locally
        this->mipLevels = givenMipLevels;
        return *this;
    }

    Image::Builder& Image::Builder::SetFormat(const VkFormat givenFormat)
    {
        // Save the given format locally
        this->format = givenFormat;
        return *this;
    }

    Image::Builder& Image::Builder::SetUsageFlags(const VkImageUsageFlags givenUsageFlags)
    {
        // Save the given usage flags locally
        this->usageFlags = givenUsageFlags;
        return *this;
    }

    Image::Builder& Image::Builder::SetMemoryFlags(const VkMemoryPropertyFlags givenMemoryFlags)
    {
        // Save the given memory flags locally
        this->memoryFlags = givenMemoryFlags;
        return *this;
    }

    Image::Builder& Image::Builder::SetImageTiling(const VkImageTiling givenImageTiling)
    {
        // Save the given tiling locally
        this->imageTiling = givenImageTiling;
        return *this;
    }

    Image::Builder& Image::Builder::SetSampling(const VkSampleCountFlagBits givenSampling)
    {
        // Save the given sampling locally
        this->sampling = givenSampling;
        return *this;
    }

    std::unique_ptr<Image> Image::Builder::Build() const
    {
        // Build and return the image
        return std::make_unique<Image>(dimensions, mipLevels, sampling, format, imageTiling, usageFlags, memoryFlags);
    }

    void Image::CreateImageView(const VkImageAspectFlags givenAspectFlags)
    {
        // Check if an image view has already been generated
        if (imageViewGenerated)
        {
            VulkanDebugger::ThrowWarning("Trying to create an image view for an image with an already existing view. Process automatically suspended");
            return;
        }

        // Set up image view creation info
        VkImageViewCreateInfo imageViewCreateInfo;
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
        VulkanDebugger::CheckResults(
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
        VkImageMemoryBarrier imageMemoryBarrier;
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
            // If transitioning from transfer destination to shader readable...
        else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStage =VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage =VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
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
            VulkanDebugger::ThrowError("Transitioning images from [" + std::to_string(layout) + "] to [" + std::to_string(newLayout) + "] is not supported");
        }

        // Bind the pipeline barrier
        vkCmdPipelineBarrier(
                commandBuffer,
                srcStage, dstStage,		            // Pipeline stages (match to src and dst AccessMasks)
                0,						                        // Dependency flags
                0, nullptr,				        // Memory Barrier count + data
                0, nullptr,			// Buffer Memory Barrier count + data
                1, &imageMemoryBarrier	// Image Memory Barrier count + data
        );

        // End command buffer
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);

        // Change the current layout
        this->layout = newLayout;
    }

    void Image::DestroyVulkanImage()
    {
       vkDestroyImage(VulkanCore::GetLogicalDevice(), this->vkImage, nullptr);
       vkFreeMemory(VulkanCore::GetLogicalDevice(), this->vkImageMemory, nullptr);
    }

    void Image::DestroyVulkanImageView()
    {
        if (imageViewGenerated) vkDestroyImageView(VulkanCore::GetLogicalDevice(), this->vkImageView, nullptr);
    }

    /* --- CONSTRUCTORS --- */

    Image::Image(const glm::vec3 givenDimensions, const uint32_t givenMipLevels, VkSampleCountFlagBits givenSampling, const VkFormat givenFormat, const VkImageTiling imageTiling, const VkImageUsageFlags usageFlags, const VkMemoryPropertyFlags propertyFlags)
        : dimensions(givenDimensions), mipLevels(givenMipLevels), sampling(givenSampling), format(givenFormat)
    {
        // Set up image creation info
        VkImageCreateInfo imageCreateInfo;
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;

        imageCreateInfo.extent.width = static_cast<uint32_t>(givenDimensions.x);
        imageCreateInfo.extent.height = static_cast<uint32_t>(givenDimensions.y);
        imageCreateInfo.extent.depth = static_cast<uint32_t>(givenDimensions.z);

        imageCreateInfo.mipLevels = givenMipLevels;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = givenFormat;
        imageCreateInfo.tiling = imageTiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usageFlags;
        imageCreateInfo.samples = givenSampling;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Create the Vulkan image
        VulkanDebugger::CheckResults(
                vkCreateImage(VulkanCore::GetLogicalDevice(), &imageCreateInfo, nullptr, &vkImage),
                "Failed to create image with dimensions of [" + std::to_string(givenDimensions.x) + ", " + std::to_string(givenDimensions.y) + ", " + std::to_string(givenDimensions.z) + "], format [" + std::to_string(givenFormat) + "], [" +
                 std::to_string(givenMipLevels) + "] mip levels, and sampling of [" + std::to_string(givenSampling) + "]"
        );

        // Retrieve its memory requirements
        VkMemoryRequirements imageMemoryRequirements;
        vkGetImageMemoryRequirements(VulkanCore::GetLogicalDevice(), vkImage, &imageMemoryRequirements);

        // Set up image memory allocation info
        VkMemoryAllocateInfo imageMemoryAllocateInfo;
        imageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        imageMemoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
        imageMemoryAllocateInfo.memoryTypeIndex = VulkanUtilities::FindMemoryTypeIndex(imageMemoryRequirements.memoryTypeBits, propertyFlags);

        // Allocate the image to memory
        VulkanDebugger::CheckResults(
            vkAllocateMemory(VulkanCore::GetLogicalDevice(), &imageMemoryAllocateInfo, nullptr, &vkImageMemory),
            "Failed to allocate memory for image with dimensions of [" + std::to_string(givenDimensions.x) + ", " + std::to_string(givenDimensions.y) + ", " + std::to_string(givenDimensions.z) + "], format [" + std::to_string(givenFormat) + "], [" +
            std::to_string(givenMipLevels) + "] mip levels, and sampling of [" + std::to_string(givenSampling) + "]"
        );

        // Bind the image to its corresponding memory
        vkBindImageMemory(VulkanCore::GetLogicalDevice(), vkImage, vkImageMemory, 0);
    }

    /* --- DESTRUCTOR --- */

    Image::~Image()
    {
        DestroyVulkanImage();
        DestroyVulkanImageView();
    }
}