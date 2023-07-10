//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Sampler.h"

#include "../VK.h"

namespace Sierra::Rendering
{
    const UniquePtr<Sampler> Sampler::Default;

    /* --- CONSTRUCTORS --- */

    Sampler::Sampler(const SamplerCreateInfo &createInfo)
        : createInfo(createInfo)
    {
        // Get the sampler filter based on whether bilinear filtering is enabled
        VkFilter samplerFilter = createInfo.applyBilinearFiltering ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

        // Set up the sampler creation info
        VkSamplerCreateInfo vkSamplerCreateInfo{};
        vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vkSamplerCreateInfo.minFilter = samplerFilter;
        vkSamplerCreateInfo.magFilter = samplerFilter;
        vkSamplerCreateInfo.addressModeU = static_cast<VkSamplerAddressMode>(createInfo.addressMode);
        vkSamplerCreateInfo.addressModeV = static_cast<VkSamplerAddressMode>(createInfo.addressMode);
        vkSamplerCreateInfo.addressModeW = static_cast<VkSamplerAddressMode>(createInfo.addressMode);
        vkSamplerCreateInfo.borderColor = static_cast<VkBorderColor>(createInfo.borderColor);
        vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        vkSamplerCreateInfo.compareEnable = VK_TRUE;
        vkSamplerCreateInfo.compareOp = static_cast<VkCompareOp>(createInfo.compareOp);
        vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        vkSamplerCreateInfo.mipLodBias = 0.0f;
        vkSamplerCreateInfo.minLod = createInfo.minLod;
        vkSamplerCreateInfo.maxLod = createInfo.maxLod;
        vkSamplerCreateInfo.anisotropyEnable = createInfo.enableAnisotropy;
        vkSamplerCreateInfo.maxAnisotropy = createInfo.enableAnisotropy ? VK::GetDevice()->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy : 0.0f;

        // Create the Vulkan sampler
        VK_ASSERT(
            vkCreateSampler(VK::GetLogicalDevice(), &vkSamplerCreateInfo, nullptr, &vkSampler),
            "Failed to create image sampler"
        );
    }

    UniquePtr<Sampler> Sampler::Create(const SamplerCreateInfo &createInfo)
    {
        return std::make_unique<Sampler>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void Sampler::Initialize()
    {
        MODIFY_CONST(UniquePtr<Sampler>, Default, Sampler::Create({ .applyBilinearFiltering = false }));
    }

    void Sampler::Shutdown()
    {
        Default->Destroy();
    }

    /* --- DESTRUCTOR --- */

    void Sampler::Destroy()
    {
        if (vkSampler == VK_NULL_HANDLE) return;

        // Destroy the Vulkan sampler
        vkDestroySampler(VK::GetLogicalDevice(), vkSampler, nullptr);

        vkSampler = VK_NULL_HANDLE;
    }

}