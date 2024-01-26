//
// Created by Nikolay Kanchevski on 7.12.23.
//

#include "VulkanImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanImage::VulkanImage(const VulkanDevice &device, const ImageCreateInfo &createInfo)
        : Image(createInfo), VulkanResource(createInfo.name), device(device), usageFlags(ImageUsageToVkImageUsageFlags(createInfo.usage))
    {
        SR_ERROR_IF(!device.IsImageSamplingSupported(createInfo.sampling), "[Vulkan]: Cannot create image [{0}] with unsupported sampling! Make sure to use Device::IsImageSamplingSupported() to query image sampling support.", GetName());
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
        imageCreateInfo.mipLevels = GetMipLevelCount();
        imageCreateInfo.arrayLayers = GetLayerCount();
        imageCreateInfo.samples = ImageSamplingToVkSampleCountFlags(GetSampling());
        imageCreateInfo.tiling = createInfo.usage != ImageUsage::SourceTransfer ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
        imageCreateInfo.usage = usageFlags;
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
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = GetMipLevelCount();
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = GetLayerCount();

        // Create the image view
        result = device.GetFunctionTable().vkCreateImageView(device.GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create image view for image [{0}]! Error code: {1}.", GetName(), result);

        // Set object names
        device.SetObjectName(image, VK_OBJECT_TYPE_IMAGE, GetName());
        device.SetObjectName(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, "Image view of [" + GetName() + "]");
    }

    VulkanImage::VulkanImage(const VulkanDevice &device, const SwapchainImageCreateInfo &createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainVkFormatToImageFormat(createInfo.format), .memoryLocation = ImageMemoryLocation::Device, .usage = ImageUsage::SourceTransfer | ImageUsage::ColorAttachment }), VulkanResource(createInfo.name),
          device(device), image(createInfo.image), usageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT), aspectFlags(VK_IMAGE_ASPECT_COLOR_BIT), swapchainImage(true)
    {
        SR_ERROR_IF(createInfo.image == VK_NULL_HANDLE, "[Vulkan]: Null texture pointer passed upon swapchain image [{0}] creation!", GetName());

        // Set up image view create info
        VkImageViewCreateInfo imageViewCreateInfo = { };
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = createInfo.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        // Create the image view
        const VkResult result = device.GetFunctionTable().vkCreateImageView(device.GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create image view for image [{0}]! Error code: {1}.", GetName(), result);

        // Set object names
        device.SetObjectName(image, VK_OBJECT_TYPE_IMAGE, GetName());
        device.SetObjectName(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, "Image view of [" + GetName() + "]");
    }

    /* --- DESTRUCTOR --- */

    VulkanImage::~VulkanImage()
    {
        device.GetFunctionTable().vkDestroyImageView(device.GetLogicalDevice(), imageView, nullptr);
        if (!swapchainImage) vmaDestroyImage(device.GetMemoryAllocator(), image, allocation);
    }

    /* --- PRIVATE METHODS --- */

    ImageFormat VulkanImage::SwapchainVkFormatToImageFormat(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_B8G8R8A8_UNORM:          return  { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::UNorm8 };
            case VK_FORMAT_B8G8R8A8_SRGB:           return  { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::SRGB8 };
            case VK_FORMAT_R8G8B8A8_UNORM:          return  { .channels = ImageChannels::RGBA, .memoryType = ImageMemoryType::UNorm8 };
            case VK_FORMAT_R8G8B8A8_SRGB:           return  { .channels = ImageChannels::RGBA, .memoryType = ImageMemoryType::SRGB8 };
            case VK_FORMAT_R16G16B16_UNORM:         return  { .channels = ImageChannels::RGBA, .memoryType = ImageMemoryType::UNorm8 };
            case VK_FORMAT_R16G16B16_SFLOAT:        return  { .channels = ImageChannels::RGBA, .memoryType = ImageMemoryType::Float16 };
            default:                                break;
        }

        SR_ERROR("[Vulkan]: Cannot determine image format of invalid swapchain VkFormat!");
        return { };
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
                    case ImageMemoryType::UNorm8:       return VK_FORMAT_R8_UNORM;
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
                }
            }
            case ImageChannels::RG:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8G8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8G8_UINT;
                    case ImageMemoryType::UNorm8:       return VK_FORMAT_R8G8_UNORM;
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
                }
            }
            case ImageChannels::RGB:
            {

                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8G8B8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8G8B8_UINT;
                    case ImageMemoryType::UNorm8:       return VK_FORMAT_R8G8B8_UNORM;
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
                }
            }
            case ImageChannels::RGBA:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return VK_FORMAT_R8G8B8A8_SINT;
                    case ImageMemoryType::UInt8:        return VK_FORMAT_R8G8B8A8_UINT;
                    case ImageMemoryType::UNorm8:       return VK_FORMAT_R8G8B8A8_UNORM;
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
                }
            }
            case ImageChannels::BGRA:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::UNorm8:       return VK_FORMAT_B8G8R8A8_UNORM;
                    case ImageMemoryType::SRGB8:        return VK_FORMAT_B8G8R8A8_SRGB;
                    default:                            break;
                }
            }
            case ImageChannels::D:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::UNorm16:   return VK_FORMAT_D16_UNORM;
                    case ImageMemoryType::Float32:   return VK_FORMAT_D32_SFLOAT;
                    default:                         break;
                }
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
        }

        return VMA_MEMORY_USAGE_AUTO;
    }

}