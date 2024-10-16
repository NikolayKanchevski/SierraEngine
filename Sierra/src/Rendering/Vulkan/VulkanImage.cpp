//
// Created by Nikolay Kanchevski on 7.12.23.
//

#include "VulkanImage.h"

#include "VulkanResultHandler.h"

namespace Sierra
{

    namespace
    {
        ImageFormat SwapchainVkFormatToImageFormat(const VkFormat format)
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

            return ImageFormat::Undefined;
        }
    }

    /* --- CONVERSIONS --- */

    VkImageType ImageTypeToVkImageType(const ImageType type) noexcept
    {
        switch (type)
        {
            case ImageType::Line:           return VK_IMAGE_TYPE_1D;
            case ImageType::Plane:
            case ImageType::Cube:           return VK_IMAGE_TYPE_2D;
            case ImageType::Volume:         return VK_IMAGE_TYPE_3D;
        }

        return VK_IMAGE_TYPE_2D;
    }

    VkImageViewType ImageTypeToVkImageViewType(const ImageType type, const uint32 layerCount) noexcept
    {
        switch (type)
        {
            case ImageType::Line:           return layerCount == 1 ? VK_IMAGE_VIEW_TYPE_1D : VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            case ImageType::Plane:          return layerCount == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case ImageType::Volume:         return VK_IMAGE_VIEW_TYPE_3D;
            case ImageType::Cube:           return layerCount / 6 == 1 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        }

        return VK_IMAGE_VIEW_TYPE_2D;
    }

    VkFormat ImageFormatToVkFormat(const ImageFormat format) noexcept
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

        return VK_FORMAT_UNDEFINED;
    }

    VkImageUsageFlags ImageUsageToVkImageUsageFlags(const ImageUsage usage) noexcept
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

    VkSampleCountFlagBits ImageSamplingToVkSampleCountFlags(const ImageSampling sampling) noexcept
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

    VkComponentSwizzle ImageComponentSwizzleToVkComponentSwizzle(const ImageComponentSwizzle componentSwizzle) noexcept
    {
        switch (componentSwizzle)
        {
            case ImageComponentSwizzle::Identity:       return VK_COMPONENT_SWIZZLE_IDENTITY;
            case ImageComponentSwizzle::Zero:           return VK_COMPONENT_SWIZZLE_ZERO;
            case ImageComponentSwizzle::One:            return VK_COMPONENT_SWIZZLE_ONE;
            case ImageComponentSwizzle::Red:            return VK_COMPONENT_SWIZZLE_R;
            case ImageComponentSwizzle::Green:          return VK_COMPONENT_SWIZZLE_G;
            case ImageComponentSwizzle::Blue:           return VK_COMPONENT_SWIZZLE_B;
            case ImageComponentSwizzle::Alpha:          return VK_COMPONENT_SWIZZLE_A;
        }

        return VK_COMPONENT_SWIZZLE_IDENTITY;
    }

    VmaMemoryUsage ImageMemoryLocationToVmaMemoryUsage(const ImageMemoryLocation memoryLocation) noexcept
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::CPU:      return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            case ImageMemoryLocation::GPU:      return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        }

        return VMA_MEMORY_USAGE_AUTO;
    }

    /* --- CONSTRUCTORS --- */

    VulkanImage::VulkanImage(const VulkanDevice& device, const ImageCreateInfo& createInfo)
        : Image(createInfo), device(device), name(createInfo.name), width(createInfo.width), height(createInfo.height), depth(createInfo.depth), format(createInfo.format), levelCount(createInfo.levelCount), layerCount(createInfo.layerCount), sampling(createInfo.sampling), usageFlags(ImageUsageToVkImageUsageFlags(createInfo.usage))
    {
        SR_THROW_IF(createInfo.type == ImageType::Line && createInfo.width > device.GetLimits().maxLineImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max line image dimensions", name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxLineImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Line && createInfo.height > device.GetLimits().maxLineImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max line image dimensions", name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxLineImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Line && createInfo.depth > device.GetLimits().maxLineImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max line image dimensions", name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxLineImageDimensions));

        SR_THROW_IF(createInfo.type == ImageType::Plane && createInfo.width > device.GetLimits().maxPlaneImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max plane image dimensions", name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxPlaneImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Plane && createInfo.height > device.GetLimits().maxPlaneImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max plane image dimensions", name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxPlaneImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Plane && createInfo.depth > device.GetLimits().maxPlaneImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max plane image dimensions", name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxPlaneImageDimensions));

        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.width > device.GetLimits().maxVolumeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max volume image dimensions", name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxVolumeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.height > device.GetLimits().maxVolumeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max volume image dimensions", name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxVolumeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.depth > device.GetLimits().maxVolumeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max volume image dimensions", name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxVolumeImageDimensions));

        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.width > device.GetLimits().maxCubeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max cube image dimensions", name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxCubeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.height > device.GetLimits().maxCubeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max cube image dimensions", name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxCubeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.depth > device.GetLimits().maxCubeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max cube image dimensions", name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxCubeImageDimensions));

        SR_THROW_IF(!device.IsImageFormatSupported(createInfo.format, createInfo.usage), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create image [{1}] with unsupported format - use Device::IsImageFormatSupported() to query support", device.GetName(), name)));
        SR_THROW_IF(!device.IsImageSamplingSupported(createInfo.sampling), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create image [{1}] with unsupported sampling - use Device::IsImageSamplingSupported() to query support", device.GetName(), name)));

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
        VkResult result = vmaCreateImage(device.GetVulkanMemoryAllocator(), &imageCreateInfo, &allocationCreateInfo, &image, &allocation, nullptr);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create image [{0}]", name));

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
                .r = ImageComponentSwizzleToVkComponentSwizzle(createInfo.redSwizzle),
                .g = ImageComponentSwizzleToVkComponentSwizzle(createInfo.greenSwizzle),
                .b = ImageComponentSwizzleToVkComponentSwizzle(createInfo.blueSwizzle),
                .a = ImageComponentSwizzleToVkComponentSwizzle(createInfo.alphaSwizzle)
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
        result = device.GetFunctionTable().vkCreateImageView(device.GetVulkanDevice(), &imageViewCreateInfo, nullptr, &imageView);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create image [{0}], as creation of image view failed", name));

        // Set object names
        device.SetResourceName(image, VK_OBJECT_TYPE_IMAGE, name);
        device.SetResourceName(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, SR_FORMAT("Image view of image [{0}]", name));
    }

    VulkanImage::VulkanImage(const VulkanDevice& device, const SwapchainImageCreateInfo& createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainVkFormatToImageFormat(createInfo.format), .usage = ImageUsage::SourceMemory | ImageUsage::ColorAttachment, .memoryLocation = ImageMemoryLocation::GPU }), device(device),
          width(createInfo.width), height(createInfo.height), format(SwapchainVkFormatToImageFormat(createInfo.format)),
          name(createInfo.name), image(createInfo.image), usageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT), aspectFlags(VK_IMAGE_ASPECT_COLOR_BIT), swapchainImage(true)
    {
        SR_THROW_IF(createInfo.image == nullptr, InvalidValueError(SR_FORMAT("Cannot create swapchain image [{0}], as specified texture must not be nullptr", name)));
        SR_THROW_IF(createInfo.format == VK_FORMAT_UNDEFINED, InvalidValueError(SR_FORMAT("Cannot create swapchain image [{0}], as format texture must not be [VK_FORMAT_UNDEFINED]", name)));

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
        const VkResult result = device.GetFunctionTable().vkCreateImageView(device.GetVulkanDevice(), &imageViewCreateInfo, nullptr, &imageView);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create swapchain image [{0}], as creation of image view failed", name));

        // Set object names
        device.SetResourceName(image, VK_OBJECT_TYPE_IMAGE, name);
        device.SetResourceName(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, SR_FORMAT("Image view of image [{0}]", name));
    }

    /* --- DESTRUCTOR --- */

    VulkanImage::~VulkanImage() noexcept
    {
        device.GetFunctionTable().vkDestroyImageView(device.GetVulkanDevice(), imageView, nullptr);
        if (!swapchainImage) vmaDestroyImage(device.GetVulkanMemoryAllocator(), image, allocation);
    }

}