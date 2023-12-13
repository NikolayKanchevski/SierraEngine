//
// Created by Nikolay Kanchevski on 7.12.23.
//

#include "VulkanImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanImage::VulkanImage(const VulkanDevice &device, const ImageCreateInfo &createInfo)
        : Image(createInfo), VulkanResource(createInfo.name), device(device), sampling(createInfo.sampling)
    {
        // Get device's sampling support
        const ImageSampling highestSampling = GetUsage() & ImageUsage::DepthAttachment ? device.GetHighestDepthSampling() : device.GetHighestColorSampling();
        if (static_cast<uint32>(createInfo.sampling) > static_cast<uint32>(highestSampling))
        {
            sampling = highestSampling;
            SR_WARNING("[Vulkan]: Cannot create image [{0}] with sampling of [{1}], as the highest supported is [{2}]! Sampling was automatically lowered.", GetName(), static_cast<uint32>(createInfo.sampling), static_cast<uint32>(highestSampling));
        }

        SR_ERROR_IF(!device.IsImageConfigurationSupported(createInfo.format, createInfo.usage), "[Vulkan]: Cannot create image [{0}] with unsupported format! Use Device::IsImageConfigurationSupported() to query format support.", GetName());

        // Set up image create info
        VkImageCreateInfo imageCreateInfo = { };
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.flags = static_cast<uint32>(createInfo.type == ImageType::Cube) * VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = ImageFormatToVkFormat(createInfo.format);
        imageCreateInfo.extent.width = GetWidth();
        imageCreateInfo.extent.height = GetHeight();
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = GetMipLevels();
        imageCreateInfo.arrayLayers = GetLayerCount();
        imageCreateInfo.samples = ImageSamplingToVkSampleCountFlags(sampling);
        imageCreateInfo.tiling = GetUsage() != ImageUsage::SourceTransfer ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
        imageCreateInfo.usage = ImageUsageToVkImageUsageFlags(createInfo.usage);
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        // Set up image allocation info
        VmaAllocationCreateInfo allocationCreateInfo = { };
        allocationCreateInfo.usage = ImageMemoryLocationToVmaMemoryUsage(createInfo.memoryLocation);
        allocationCreateInfo.memoryTypeBits = std::numeric_limits<uint32>::max();
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate image
        VkResult result = vmaCreateImage(device.GetMemoryAllocator(), &imageCreateInfo, &allocationCreateInfo, &image, &allocation, nullptr);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create image [{0}]! Error code: {1}.", GetName(), result);

        // Determine aspect flags
        VkImageAspectFlags aspectFlags = 0;
        if (createInfo.usage & ImageUsage::DepthAttachment) aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        else aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        // Determine view type
        VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;
        if (createInfo.type == ImageType::Cube) imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;

        // Set up image view create info
        VkImageViewCreateInfo imageViewCreateInfo = { };
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = imageViewType;
        imageViewCreateInfo.format = imageCreateInfo.format;
        imageViewCreateInfo.subresourceRange = {
            .aspectMask = aspectFlags,
            .baseMipLevel = 0,
            .levelCount = GetMipLevels(),
            .baseArrayLayer = 0,
            .layerCount = GetLayerCount()
        };

        // Create the image view
        result = device.GetFunctionTable().vkCreateImageView(device.GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create image view for image [{0}]! Error code: {1}.", GetName(), result);
    }

    /* --- DESTRUCTOR --- */

    void VulkanImage::Destroy()
    {
        device.GetFunctionTable().vkDestroyImageView(device.GetLogicalDevice(), imageView, nullptr);
        if (image != VK_NULL_HANDLE) vmaDestroyImage(device.GetMemoryAllocator(), image, allocation);
    }

    /* --- CONVERSIONS --- */

    VkFormat VulkanImage::ImageFormatToVkFormat(const ImageFormat format)
    {
        switch (format.channels)
        {
            case ImageChannels::R:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8_UINT;
                    case ImageMemoryType::SRGB8:        return VK_FORMAT_R8_SRGB;
                    case ImageMemoryType::Int16:        return VK_FORMAT_R16_SINT;
                    case ImageMemoryType::UInt16:       return VK_FORMAT_R16_UINT;
                    case ImageMemoryType::Norm16:       return VK_FORMAT_R16_SNORM;
                    case ImageMemoryType::UNorm16:      return VK_FORMAT_R16_UNORM;
                    case ImageMemoryType::Float16:      return VK_FORMAT_R16_SFLOAT;
                    case ImageMemoryType::Int32:        return VK_FORMAT_R32_SINT;
                    case ImageMemoryType::UInt32:       return VK_FORMAT_R32_UINT;
                    case ImageMemoryType::Float32:      return VK_FORMAT_R32_SFLOAT;
                    case ImageMemoryType::Int64:        return VK_FORMAT_R64_SINT;
                    case ImageMemoryType::UInt64:       return VK_FORMAT_R64_UINT;
                    case ImageMemoryType::Float64:      return VK_FORMAT_R64_SFLOAT;
                    default:                            break;
                }
            }
            case ImageChannels::RG:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8G8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8G8_UINT;
                    case ImageMemoryType::SRGB8:        return VK_FORMAT_R8G8_SRGB;
                    case ImageMemoryType::Int16:        return VK_FORMAT_R16G16_SINT;
                    case ImageMemoryType::UInt16:       return VK_FORMAT_R16G16_UINT;
                    case ImageMemoryType::Norm16:       return VK_FORMAT_R16G16_SNORM;
                    case ImageMemoryType::UNorm16:      return VK_FORMAT_R16G16_UNORM;
                    case ImageMemoryType::Float16:      return VK_FORMAT_R16G16_SFLOAT;
                    case ImageMemoryType::Int32:        return VK_FORMAT_R32G32_SINT;
                    case ImageMemoryType::UInt32:       return VK_FORMAT_R32G32_UINT;
                    case ImageMemoryType::Float32:      return VK_FORMAT_R32G32_SFLOAT;
                    case ImageMemoryType::Int64:        return VK_FORMAT_R64G64_SINT;
                    case ImageMemoryType::UInt64:       return VK_FORMAT_R64G64_UINT;
                    case ImageMemoryType::Float64:      return VK_FORMAT_R64G64_SFLOAT;
                    default:                            break;
                }
            }
            case ImageChannels::RGB:
            {

                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8G8B8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8G8B8_UINT;
                    case ImageMemoryType::SRGB8:        return VK_FORMAT_R8G8B8_SRGB;
                    case ImageMemoryType::Int16:        return VK_FORMAT_R16G16B16_SINT;
                    case ImageMemoryType::UInt16:       return VK_FORMAT_R16G16B16_UINT;
                    case ImageMemoryType::Norm16:       return VK_FORMAT_R16G16B16_SNORM;
                    case ImageMemoryType::UNorm16:      return VK_FORMAT_R16G16B16_UNORM;
                    case ImageMemoryType::Float16:      return VK_FORMAT_R16G16B16_SFLOAT;
                    case ImageMemoryType::Int32:        return VK_FORMAT_R32G32B32_SINT;
                    case ImageMemoryType::UInt32:       return VK_FORMAT_R32G32B32_UINT;
                    case ImageMemoryType::Float32:      return VK_FORMAT_R32G32B32_SFLOAT;
                    case ImageMemoryType::Int64:        return VK_FORMAT_R64G64B64_SINT;
                    case ImageMemoryType::UInt64:       return VK_FORMAT_R64G64B64_UINT;
                    case ImageMemoryType::Float64:      return VK_FORMAT_R64G64B64_SFLOAT;
                    default:                            break;
                }
            }
            case ImageChannels::RGBA:
            {

                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8G8B8A8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8G8B8A8_UINT;
                    case ImageMemoryType::SRGB8:        return VK_FORMAT_R8G8B8A8_SRGB;
                    case ImageMemoryType::Int16:        return VK_FORMAT_R16G16B16A16_SINT;
                    case ImageMemoryType::UInt16:       return VK_FORMAT_R16G16B16A16_UINT;
                    case ImageMemoryType::Norm16:       return VK_FORMAT_R16G16B16A16_SNORM;
                    case ImageMemoryType::UNorm16:      return VK_FORMAT_R16G16B16A16_UNORM;
                    case ImageMemoryType::Float16:      return VK_FORMAT_R16G16B16A16_SFLOAT;
                    case ImageMemoryType::Int32:        return VK_FORMAT_R32G32B32A32_SINT;
                    case ImageMemoryType::UInt32:       return VK_FORMAT_R32G32B32A32_UINT;
                    case ImageMemoryType::Float32:      return VK_FORMAT_R32G32B32A32_SFLOAT;
                    case ImageMemoryType::Int64:        return VK_FORMAT_R64G64B64A64_SINT;
                    case ImageMemoryType::UInt64:       return VK_FORMAT_R64G64B64A64_UINT;
                    case ImageMemoryType::Float64:      return VK_FORMAT_R64G64B64A64_SFLOAT;
                    default:                            break;
                }
            }
            default:
            {
                break;
            }
        }

        SR_ERROR("[Vulkan]: Cannot determine image format of invalid channel and memory configuration!");
        return VK_FORMAT_UNDEFINED;
    }

    VkSampleCountFlagBits VulkanImage::ImageSamplingToVkSampleCountFlags(const ImageSampling sampling)
    {
        switch (sampling)
        {
            case ImageSampling::x1:         return VK_SAMPLE_COUNT_1_BIT;
            case ImageSampling::x2:         return VK_SAMPLE_COUNT_2_BIT;
            case ImageSampling::x4:         return VK_SAMPLE_COUNT_4_BIT;
            case ImageSampling::x8:         return VK_SAMPLE_COUNT_8_BIT;
            case ImageSampling::x16:        return VK_SAMPLE_COUNT_16_BIT;
            case ImageSampling::x32:        return VK_SAMPLE_COUNT_32_BIT;
            case ImageSampling::x64:        return VK_SAMPLE_COUNT_64_BIT;
            default:                        break;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkImageUsageFlags VulkanImage::ImageUsageToVkImageUsageFlags(const ImageUsage usage)
    {
        VkImageUsageFlags usageFlags = 0;
        if (usage & ImageUsage::SourceTransfer)                usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (usage & ImageUsage::DestinationTransfer)           usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (usage & ImageUsage::Storage)                       usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (usage & ImageUsage::Sampled)                       usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (usage & ImageUsage::ColorAttachment)               usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (usage & ImageUsage::DepthAttachment)               usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (usage & ImageUsage::InputAttachment)               usageFlags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        if (usage & ImageUsage::TransientAttachment)           usageFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        return usageFlags;
    }

    VmaMemoryUsage VulkanImage::ImageMemoryLocationToVmaMemoryUsage(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::Host:         return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            case ImageMemoryLocation::Device:       return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            case ImageMemoryLocation::Auto:         return VMA_MEMORY_USAGE_AUTO;
            default:                                break;
        }

        return VMA_MEMORY_USAGE_AUTO;
    }

}