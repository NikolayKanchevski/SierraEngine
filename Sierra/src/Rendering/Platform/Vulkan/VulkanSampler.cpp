//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "VulkanSampler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanSampler::VulkanSampler(const VulkanDevice& device, const SamplerCreateInfo& createInfo)
        : Sampler(createInfo), device(device), name(createInfo.name)
    {
        SR_ERROR_IF(!device.IsSamplerAnisotropySupported(createInfo.anisotropy), "[Vulkan]: Cannot create sampler [{0}] with unsupported sample mode! Use Device::IsSamplerAnisotropySupported() to query support.", name);

        // Set up sampler create info
        const VkSamplerCreateInfo samplerCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = SamplerSampleModeToVkFilter(createInfo.filter),
            .minFilter = SamplerSampleModeToVkFilter(createInfo.filter),
            .mipmapMode = SamplerSampleModeToVkSamplerMipMapMode(createInfo.filter),
            .addressModeU = SamplerExtendModeToVkSamplerAddressMode(createInfo.extendMode),
            .addressModeV = SamplerExtendModeToVkSamplerAddressMode(createInfo.extendMode),
            .addressModeW = SamplerExtendModeToVkSamplerAddressMode(createInfo.extendMode),
            .mipLodBias = 0.0f,
            .anisotropyEnable = createInfo.anisotropy != SamplerAnisotropy::x1,
            .maxAnisotropy = SamplerAnisotropyToFloat32(createInfo.anisotropy),
            .compareEnable = createInfo.compareOperation != SamplerCompareOperation::None,
            .compareOp = createInfo.anisotropy != SamplerAnisotropy::x1 ? SamplerCompareOperationToVkCompareOp(createInfo.compareOperation) : VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = static_cast<float32>(createInfo.highestSampledLevel),
            .borderColor = SamplerBorderColorToVkBorderColor(createInfo.borderColor),
            .unnormalizedCoordinates = VK_FALSE,
        };

        // Create sampler
        const VkResult result = device.GetFunctionTable().vkCreateSampler(device.GetLogicalDevice(), &samplerCreateInfo, nullptr, &sampler);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create sampler [{0}]! Error code: {1}.", name, static_cast<int32>(result));

        // Assign name
        device.SetResourceName(sampler, VK_OBJECT_TYPE_SAMPLER, name);
    }

    /* --- DESTRUCTOR --- */

    VulkanSampler::~VulkanSampler()
    {
        device.GetFunctionTable().vkDestroySampler(device.GetLogicalDevice(), sampler, nullptr);
    }

    /* --- CONVERSIONS --- */

    VkFilter VulkanSampler::SamplerSampleModeToVkFilter(const SamplerFilter sampleMode)
    {
        switch (sampleMode)
        {
            case SamplerFilter::Linear:     return VK_FILTER_LINEAR;
            case SamplerFilter::Nearest:   return VK_FILTER_NEAREST;
        }

        return VK_FILTER_NEAREST;
    }

    VkSamplerMipmapMode VulkanSampler::SamplerSampleModeToVkSamplerMipMapMode(const SamplerFilter sampleMode)
    {
        switch (sampleMode)
        {
            case SamplerFilter::Linear:         return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            case SamplerFilter::Nearest:        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }

        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    }

    VkSamplerAddressMode VulkanSampler::SamplerExtendModeToVkSamplerAddressMode(const SamplerAddressMode extendMode)
    {
        switch (extendMode)
        {
            case SamplerAddressMode::Repeat:             return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case SamplerAddressMode::MirroredRepeat:     return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case SamplerAddressMode::ClampToEdge:        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case SamplerAddressMode::ClampToBorder:      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }

        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }

    float32 VulkanSampler::SamplerAnisotropyToFloat32(const SamplerAnisotropy anisotropy)
    {
        switch (anisotropy)
        {
            case SamplerAnisotropy::x1:     return 1.0f;
            case SamplerAnisotropy::x2:     return 2.0f;
            case SamplerAnisotropy::x4:     return 4.0f;
            case SamplerAnisotropy::x8:     return 8.0f;
            case SamplerAnisotropy::x16:    return 16.0f;
            case SamplerAnisotropy::x32:    return 32.0f;
            case SamplerAnisotropy::x64:    return 64.0f;
        }

        return 1.0f;
    }

    VkCompareOp VulkanSampler::SamplerCompareOperationToVkCompareOp(const SamplerCompareOperation compareOperation)
    {
        switch (compareOperation)
        {
            case SamplerCompareOperation::None:             return VK_COMPARE_OP_ALWAYS;
            case SamplerCompareOperation::Equal:            return VK_COMPARE_OP_EQUAL;
            case SamplerCompareOperation::NotEqual:         return VK_COMPARE_OP_NOT_EQUAL;
            case SamplerCompareOperation::Less:             return VK_COMPARE_OP_LESS;
            case SamplerCompareOperation::Greater:          return VK_COMPARE_OP_GREATER;
            case SamplerCompareOperation::LessOrEqual:      return VK_COMPARE_OP_LESS_OR_EQUAL;
            case SamplerCompareOperation::GreaterOrEqual:   return VK_COMPARE_OP_GREATER_OR_EQUAL;
        }

        return VK_COMPARE_OP_ALWAYS;
    }

    VkBorderColor VulkanSampler::SamplerBorderColorToVkBorderColor(const SamplerBorderColor borderColor)
    {
        switch (borderColor)
        {
            case SamplerBorderColor::Transparent:   return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            case SamplerBorderColor::White:         return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            case SamplerBorderColor::Black:         return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        }

        return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    }

}