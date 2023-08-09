//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Image.h"

#include "../Bases/VK.h"
#include "../../../Engine/Handlers/Assets/AssetManager.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), mipLevels(createInfo.mipLevels), layerCount(createInfo.layerCount), usage(createInfo.usage), sampling(createInfo.sampling), format(createInfo.format)
    {
        // Set up image creation info
        VkImageCreateInfo vkImageCreateInfo{};
        vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType = static_cast<VkImageType>(createInfo.imageType);
        vkImageCreateInfo.extent.width = width;
        vkImageCreateInfo.extent.height = height;
        vkImageCreateInfo.extent.depth = 1;
        vkImageCreateInfo.mipLevels = mipLevels;
        vkImageCreateInfo.arrayLayers = layerCount;
        vkImageCreateInfo.format = static_cast<VkFormat>(format);
        vkImageCreateInfo.tiling = static_cast<VkImageTiling>(createInfo.imageTiling);
        vkImageCreateInfo.initialLayout = static_cast<VkImageLayout>(ImageLayout::UNDEFINED);
        vkImageCreateInfo.usage = static_cast<VkImageUsageFlagBits>(usage);
        vkImageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(sampling);
        vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkImageCreateInfo.flags = static_cast<uint32>(createInfo.createFlags);

        // Set up image allocation info
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocationCreateInfo.memoryTypeBits = UINT_MAX;
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate image
        VK_ASSERT(
            vmaCreateImage(VK::GetMemoryAllocator(), &vkImageCreateInfo, &allocationCreateInfo, &vkImage, &vmaImageAllocation, nullptr),
            FORMAT_STRING("Failed to allocate memory for image with dimensions of [{0}x{1}], format [{2}], [{3}] mip levels, and sampling of [{4}]", width, height, VK_TO_STRING(sampling, Format), static_cast<int32>(mipLevels), VK_TO_STRING(sampling, SampleCountFlagBits))
        );

        CreateImageView(createInfo.createFlags);
    }

    void Image::CreateImageView(const ImageCreateFlags createFlags)
    {
        // Get aspect flags
        if (usage == ImageUsage::UNDEFINED)                                         aspectFlags = ImageAspectFlags::UNDEFINED;
        else if (IS_FLAG_PRESENT(usage, ImageUsage::DEPTH_STENCIL_ATTACHMENT))      aspectFlags = ImageAspectFlags::DEPTH;
        else                                                                        aspectFlags = ImageAspectFlags::COLOR;

        // Get view type
        VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;
        if (IS_FLAG_PRESENT(createFlags, ImageCreateFlags::CUBE_COMPATIBLE)) imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;

        // Set up image view creation info
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vkImage;
        imageViewCreateInfo.viewType = imageViewType;
        imageViewCreateInfo.format = static_cast<VkFormat>(format);
        imageViewCreateInfo.subresourceRange.aspectMask = static_cast<VkImageAspectFlagBits>(aspectFlags);
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = layerCount;

        // Create the image view
        VK_ASSERT(
            vkCreateImageView(VK::GetLogicalDevice(), &imageViewCreateInfo, nullptr, &vkImageView),
            FORMAT_STRING("Failed to create image view for an image with dimensions of [{0}x{1}], format [{2}], [{3}] mip levels, and sampling of [{4}]", width, height, VK_TO_STRING(sampling, Format), static_cast<int32>(mipLevels), VK_TO_STRING(sampling, SampleCountFlagBits))
        );
    }

    UniquePtr<Image> Image::Create(const ImageCreateInfo &createInfo)
    {
        return std::make_unique<Image>(createInfo);
    }

    Image::Image(const SwapchainImageCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), vkImage(createInfo.image), format(createInfo.format), usage(ImageUsage::COLOR_ATTACHMENT), sampling(createInfo.sampling), swapchainImage(true)
    {
        CreateImageView();
    }

    UniquePtr<Image> Image::CreateSwapchainImage(const SwapchainImageCreateInfo &createInfo)
    {
        return std::make_unique<Image>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void Image::Destroy()
    {
        vkDestroyImageView(VK::GetLogicalDevice(), vkImageView, nullptr);
        vkImageView = VK_NULL_HANDLE;

        // Swapchain images are destroyed by the swapchain
        if (!swapchainImage)
        {
            vkImage = VK_NULL_HANDLE;
            vmaDestroyImage(VK::GetMemoryAllocator(), vkImage, vmaImageAllocation);
        }
    }
}