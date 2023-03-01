//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Sampler.h"

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    const UniquePtr<Sampler> Sampler::Default;

    /* --- CONSTRUCTORS --- */

    Sampler::Sampler(const SamplerCreateInfo &samplerCreateInfo)
        : createInfo(samplerCreateInfo)
    {
        // Get the sampler filter based on whether bilinear filtering is enabled
        VkFilter samplerFilter = samplerCreateInfo.applyBilinearFiltering ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

        // Set up the sampler creation info
        VkSamplerCreateInfo vkSamplerCreateInfo{};
        vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vkSamplerCreateInfo.minFilter = samplerFilter;
        vkSamplerCreateInfo.magFilter = samplerFilter;
        vkSamplerCreateInfo.addressModeU = (VkSamplerAddressMode) samplerAddressMode;
        vkSamplerCreateInfo.addressModeV = (VkSamplerAddressMode) samplerAddressMode;
        vkSamplerCreateInfo.addressModeW = (VkSamplerAddressMode) samplerAddressMode;
        vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        vkSamplerCreateInfo.compareEnable = VK_FALSE;
        vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        vkSamplerCreateInfo.mipLodBias = 0.0f;
        vkSamplerCreateInfo.minLod = samplerCreateInfo.minLod;
        vkSamplerCreateInfo.maxLod = samplerCreateInfo.maxLod;
        vkSamplerCreateInfo.anisotropyEnable = samplerCreateInfo.maxAnisotropy > 0.0f;
        vkSamplerCreateInfo.maxAnisotropy = samplerCreateInfo.maxAnisotropy;

        // Create the Vulkan sampler
        VK_ASSERT(
            vkCreateSampler(VK::GetLogicalDevice(), &vkSamplerCreateInfo, nullptr, &vkSampler),
            fmt::format("Failed to create sampler with a LOD of [{0}, {1}] and [{2}] max anisotropy", createInfo.minLod, createInfo.maxLod, createInfo.maxAnisotropy)
        );
    }

    UniquePtr<Sampler> Sampler::Create(const SamplerCreateInfo createInfo)
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
        vkDestroySampler(VK::GetLogicalDevice(), this->vkSampler, nullptr);

        vkSampler = VK_NULL_HANDLE;
    }
}