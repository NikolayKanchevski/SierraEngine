//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Sampler.h"

#include "../Bases/VK.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    Sampler::Sampler(const SamplerCreateInfo &createInfo)
    {
        // Get the sampler filter based on whether bilinear filtering is enabled
        VkFilter samplerFilter = createInfo.applyBilinearFiltering ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

        // Set up the sampler creation info
        VkSamplerCreateInfo vkSamplerCreateInfo{};
        vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vkSamplerCreateInfo.minFilter = samplerFilter;
        vkSamplerCreateInfo.magFilter = samplerFilter;
        vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkSamplerCreateInfo.borderColor = static_cast<VkBorderColor>(createInfo.borderColor);
        vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        vkSamplerCreateInfo.compareEnable = VK_TRUE;
        vkSamplerCreateInfo.compareOp = static_cast<VkCompareOp>(createInfo.compareOp);
        vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        vkSamplerCreateInfo.mipLodBias = 0.0f;
        vkSamplerCreateInfo.minLod = 0.0f;
        vkSamplerCreateInfo.maxLod = createInfo.maxLod;
        if (createInfo.enableAnisotropy)
        {
            vkSamplerCreateInfo.anisotropyEnable = VK::GetDevice()->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy > 0.0f;
            vkSamplerCreateInfo.maxAnisotropy = VK::GetDevice()->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy / 2.0f;
        }

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

    /* --- DESTRUCTOR --- */

    void Sampler::Destroy()
    {
        // Destroy the Vulkan sampler
        vkDestroySampler(VK::GetLogicalDevice(), vkSampler, nullptr);
        vkSampler = VK_NULL_HANDLE;
    }

}