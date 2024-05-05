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
        SR_ERROR_IF(!device.IsImageSamplingSupported(createInfo.sampling), "[Vulkan]: Cannot create image [{0}] with unsupported sampling! Use Device::IsImageSamplingSupported() to query image sampling support.", GetName());
        SR_ERROR_IF(!device.IsImageFormatSupported(createInfo.format, createInfo.usage), "[Vulkan]: Cannot create image [{0}] with unsupported format! Use Device::IsImageFormatSupported() to query format support.", GetName());

        // Set up image create info
        const VkImageCreateInfo imageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = static_cast<uint32>(createInfo.type == ImageType::Cube) * VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .imageType = ImageTypeToVkImageType(createInfo.type),
            .format = ImageFormatToVkFormat(createInfo.format),
            .extent = {
                .width = createInfo.width,
                .height = createInfo.height,
                .depth = createInfo.depth
            },
            .mipLevels = createInfo.levelCount,
            .arrayLayers = createInfo.layerCount,
            .samples = ImageSamplingToVkSampleCountFlags(createInfo.sampling),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usageFlags,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        // Set up image allocation info
        const VmaAllocationCreateInfo allocationCreateInfo
        {
            .usage = ImageMemoryLocationToVmaMemoryUsage(createInfo.memoryLocation),
            .memoryTypeBits = std::numeric_limits<uint32>::max(),
            .priority = 0.5f
        };

        // Create and allocate image
        VkResult result = vmaCreateImage(device.GetMemoryAllocator(), &imageCreateInfo, &allocationCreateInfo, &image, &allocation, nullptr);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create image [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Determine aspect flags
        if (createInfo.usage & ImageUsage::DepthAttachment) aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        else aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        // Set up image view create info
        const VkImageViewCreateInfo imageViewCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = ImageTypeToVkImageViewType(createInfo.type, createInfo.layerCount),
            .format = imageCreateInfo.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = createInfo.levelCount,
                .baseArrayLayer = 0,
                .layerCount = createInfo.layerCount
            }
        };

        // Create the image view
        result = device.GetFunctionTable().vkCreateImageView(device.GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create image view for image [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Set object names
        device.SetObjectName(image, VK_OBJECT_TYPE_IMAGE, GetName());
        device.SetObjectName(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, "Image view of image [" + std::string(GetName()) + "]");
    }

    VulkanImage::VulkanImage(const VulkanDevice &device, const SwapchainImageCreateInfo &createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainVkFormatToImageFormat(createInfo.format), .usage = ImageUsage::SourceMemory | ImageUsage::ColorAttachment, .memoryLocation = ImageMemoryLocation::GPU }), VulkanResource(createInfo.name),
          device(device), image(createInfo.image), usageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT), aspectFlags(VK_IMAGE_ASPECT_COLOR_BIT), swapchainImage(true)
    {
        SR_ERROR_IF(createInfo.image == VK_NULL_HANDLE, "[Vulkan]: Null texture pointer passed upon swapchain image [{0}] creation!", GetName());

        // Set up image view create info
        const VkImageViewCreateInfo imageViewCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = createInfo.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        // Create the image view
        const VkResult result = device.GetFunctionTable().vkCreateImageView(device.GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create image view for image [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Set object names
        device.SetObjectName(image, VK_OBJECT_TYPE_IMAGE, GetName());
        device.SetObjectName(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, "Image view of image [" + std::string(GetName()) + "]");
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
            case VK_FORMAT_B8G8R8A8_UNORM:          return  ImageFormat::B8G8R8A8_UNorm;
            case VK_FORMAT_B8G8R8A8_SRGB:           return  ImageFormat::B8G8R8A8_SRGB;
            case VK_FORMAT_R8G8B8A8_UNORM:          return  ImageFormat::R8G8B8A8_UNorm;
            case VK_FORMAT_R8G8B8A8_SRGB:           return  ImageFormat::R8G8B8A8_SRGB;
            case VK_FORMAT_R16G16B16_UNORM:         return  ImageFormat::R16G16B16_UNorm;
            case VK_FORMAT_R16G16B16_SFLOAT:        return  ImageFormat::R16G16B16_Float;
            default:                                break;
        }

        SR_ERROR("[Vulkan]: Cannot determine image format of invalid swapchain VkFormat!");
        return { };
    }

    /* --- CONVERSIONS --- */

    VkImageType VulkanImage::ImageTypeToVkImageType(const ImageType type)
    {
        switch (type)
        {
            case ImageType::Line:           return VK_IMAGE_TYPE_1D;
            case ImageType::Plane:
            case ImageType::Cube:           return VK_IMAGE_TYPE_2D;
            case ImageType::Volume:         return VK_IMAGE_TYPE_3D;
        }
    }

    VkFormat VulkanImage::ImageFormatToVkFormat(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                return VK_FORMAT_UNDEFINED;

            case ImageFormat::R8_Int:                   return VK_FORMAT_R8_SINT;
            case ImageFormat::R8_UInt:                  return VK_FORMAT_R8_UINT;
            case ImageFormat::R8_Norm:                  return VK_FORMAT_R8_SNORM;
            case ImageFormat::R8_UNorm:                 return VK_FORMAT_R8_UNORM;
            case ImageFormat::R8_SRGB:                  return VK_FORMAT_R8_SRGB;
            case ImageFormat::R8G8_Int:                 return VK_FORMAT_R8G8_SINT;
            case ImageFormat::R8G8_UInt:                return VK_FORMAT_R8G8_UINT;
            case ImageFormat::R8G8_Norm:                return VK_FORMAT_R8G8_SNORM;
            case ImageFormat::R8G8_UNorm:               return VK_FORMAT_R8G8_UNORM;
            case ImageFormat::R8G8_SRGB:                return VK_FORMAT_R8G8_SRGB;
            case ImageFormat::R8G8B8_Int:               return VK_FORMAT_R8G8B8_SINT;
            case ImageFormat::R8G8B8_UInt:              return VK_FORMAT_R8G8B8_UINT;
            case ImageFormat::R8G8B8_Norm:              return VK_FORMAT_R8G8B8_SNORM;
            case ImageFormat::R8G8B8_UNorm:             return VK_FORMAT_R8G8B8_UNORM;
            case ImageFormat::R8G8B8_SRGB:              return VK_FORMAT_R8G8B8_SRGB;
            case ImageFormat::R8G8B8A8_Int:             return VK_FORMAT_R8G8B8A8_SINT;
            case ImageFormat::R8G8B8A8_UInt:            return VK_FORMAT_R8G8B8A8_UINT;
            case ImageFormat::R8G8B8A8_Norm:            return VK_FORMAT_R8G8B8A8_SNORM;
            case ImageFormat::R8G8B8A8_UNorm:           return VK_FORMAT_R8G8B8A8_UNORM;
            case ImageFormat::R8G8B8A8_SRGB:            return VK_FORMAT_R8G8B8A8_SRGB;

            case ImageFormat::R16_Int:                  return VK_FORMAT_R16_SINT;
            case ImageFormat::R16_UInt:                 return VK_FORMAT_R16_UINT;
            case ImageFormat::R16_Norm:                 return VK_FORMAT_R16_SNORM;
            case ImageFormat::R16_UNorm:                return VK_FORMAT_R16_UNORM;
            case ImageFormat::R16_Float:                return VK_FORMAT_R16_SFLOAT;
            case ImageFormat::R16G16_Int:               return VK_FORMAT_R16G16_SINT;
            case ImageFormat::R16G16_UInt:              return VK_FORMAT_R16G16_UINT;
            case ImageFormat::R16G16_Norm:              return VK_FORMAT_R16G16_SNORM;
            case ImageFormat::R16G16_UNorm:             return VK_FORMAT_R16G16_UNORM;
            case ImageFormat::R16G16_Float:             return VK_FORMAT_R16G16_SFLOAT;
            case ImageFormat::R16G16B16_Int:            return VK_FORMAT_R16G16B16_SINT;
            case ImageFormat::R16G16B16_UInt:           return VK_FORMAT_R16G16B16_UINT;
            case ImageFormat::R16G16B16_Norm:           return VK_FORMAT_R16G16B16_SNORM;
            case ImageFormat::R16G16B16_UNorm:          return VK_FORMAT_R16G16B16_UNORM;
            case ImageFormat::R16G16B16_Float:          return VK_FORMAT_R16G16B16_SFLOAT;
            case ImageFormat::R16G16B16A16_Int:         return VK_FORMAT_R16G16B16A16_SINT;
            case ImageFormat::R16G16B16A16_UInt:        return VK_FORMAT_R16G16B16A16_UINT;
            case ImageFormat::R16G16B16A16_Norm:        return VK_FORMAT_R16G16B16A16_SNORM;
            case ImageFormat::R16G16B16A16_UNorm:       return VK_FORMAT_R16G16B16A16_UNORM;
            case ImageFormat::R16G16B16A16_Float:       return VK_FORMAT_R16G16B16A16_SFLOAT;
    
            case ImageFormat::R32_Int:                  return VK_FORMAT_R32_SINT;
            case ImageFormat::R32_UInt:                 return VK_FORMAT_R32_UINT;
            case ImageFormat::R32_Float:                return VK_FORMAT_R32_SFLOAT;
            case ImageFormat::R32G32_Int:               return VK_FORMAT_R32G32_SINT;
            case ImageFormat::R32G32_UInt:              return VK_FORMAT_R32G32_UINT;
            case ImageFormat::R32G32_Float:             return VK_FORMAT_R32G32_SFLOAT;
            case ImageFormat::R32G32B32_Int:            return VK_FORMAT_R32G32B32_SINT;
            case ImageFormat::R32G32B32_UInt:           return VK_FORMAT_R32G32B32_UINT;
            case ImageFormat::R32G32B32_Float:          return VK_FORMAT_R32G32B32_SFLOAT;
            case ImageFormat::R32G32B32A32_Int:         return VK_FORMAT_R32G32B32A32_SINT;
            case ImageFormat::R32G32B32A32_UInt:        return VK_FORMAT_R32G32B32A32_UINT;
            case ImageFormat::R32G32B32A32_Float:       return VK_FORMAT_R32G32B32A32_SFLOAT;
            
            case ImageFormat::R64_Int:                  return VK_FORMAT_R64_SINT;
            case ImageFormat::R64_UInt:                 return VK_FORMAT_R64_UINT;
            case ImageFormat::R64_Float:                return VK_FORMAT_R64_SFLOAT;
            case ImageFormat::R64G64_Int:               return VK_FORMAT_R64G64_SINT;
            case ImageFormat::R64G64_UInt:              return VK_FORMAT_R64G64_UINT;
            case ImageFormat::R64G64_Float:             return VK_FORMAT_R64G64_SFLOAT;
            case ImageFormat::R64G64B64_Int:            return VK_FORMAT_R64G64B64_SINT;
            case ImageFormat::R64G64B64_UInt:           return VK_FORMAT_R64G64B64_UINT;
            case ImageFormat::R64G64B64_Float:          return VK_FORMAT_R64G64B64_SFLOAT;
            case ImageFormat::R64G64B64A64_Int:         return VK_FORMAT_R64G64B64A64_SINT;
            case ImageFormat::R64G64B64A64_UInt:        return VK_FORMAT_R64G64B64A64_UINT;
            case ImageFormat::R64G64B64A64_Float:       return VK_FORMAT_R64G64B64A64_SFLOAT;
            
            case ImageFormat::D16_UNorm:                return VK_FORMAT_D16_UNORM;
            case ImageFormat::D32_Float:                return VK_FORMAT_D32_SFLOAT;
            
            case ImageFormat::B8G8R8A8_UNorm:           return VK_FORMAT_B8G8R8A8_UNORM;
            case ImageFormat::B8G8R8A8_SRGB:            return VK_FORMAT_B8G8R8A8_SRGB;
            
            case ImageFormat::BC1_RGB_UNorm:            return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
            case ImageFormat::BC1_RGB_SRGB:             return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
            case ImageFormat::BC1_RGBA_UNorm:           return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            case ImageFormat::BC1_RGBA_SRGB:            return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
            case ImageFormat::BC3_RGBA_UNorm:           return VK_FORMAT_BC3_UNORM_BLOCK;
            case ImageFormat::BC3_RGBA_SRGB:            return VK_FORMAT_BC3_SRGB_BLOCK;
            case ImageFormat::BC4_R_Norm:               return VK_FORMAT_BC4_SNORM_BLOCK;
            case ImageFormat::BC4_R_UNorm:              return VK_FORMAT_BC4_UNORM_BLOCK;
            case ImageFormat::BC5_RG_Norm:              return VK_FORMAT_BC5_SNORM_BLOCK;
            case ImageFormat::BC5_RG_UNorm:             return VK_FORMAT_BC5_UNORM_BLOCK;
            case ImageFormat::BC6_HDR_RGB_Float:        return VK_FORMAT_BC6H_SFLOAT_BLOCK;
            case ImageFormat::BC6_HDR_RGB_UFloat:       return VK_FORMAT_BC6H_UFLOAT_BLOCK;
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGBA_UNorm:           return VK_FORMAT_BC7_UNORM_BLOCK;
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_SRGB:            return VK_FORMAT_BC7_SRGB_BLOCK;

            case ImageFormat::ASTC_4x4_UNorm:           return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
            case ImageFormat::ASTC_4x4_SRGB:            return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
            case ImageFormat::ASTC_8x8_UNorm:           return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
            case ImageFormat::ASTC_8x8_SRGB:            return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
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
        if (usage & ImageUsage::SourceMemory)               usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (usage & ImageUsage::DestinationMemory)          usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (usage & ImageUsage::Storage)                    usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (usage & ImageUsage::Sample)                     usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (usage & ImageUsage::Filter)                     usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (usage & ImageUsage::ColorAttachment)            usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (usage & ImageUsage::DepthAttachment)            usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (usage & ImageUsage::InputAttachment)            usageFlags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        if (usage & ImageUsage::TransientAttachment)        usageFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        return usageFlags;
    }

    VmaMemoryUsage VulkanImage::ImageMemoryLocationToVmaMemoryUsage(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::CPU:      return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            case ImageMemoryLocation::GPU:      return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        }

        return VMA_MEMORY_USAGE_AUTO;
    }

    VkImageViewType VulkanImage::ImageTypeToVkImageViewType(const ImageType type, const uint32 layerCount)
    {
        switch (type)
        {
            case ImageType::Line:           return layerCount == 1 ? VK_IMAGE_VIEW_TYPE_1D : VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            case ImageType::Plane:          return layerCount == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case ImageType::Volume:         return VK_IMAGE_VIEW_TYPE_3D;
            case ImageType::Cube:           return (layerCount / 6) == 1 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        }
    }

}