//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Sampler.h"
#include "../VulkanCore.h"
#include "../../../../Engine/Classes/Math.h"
#include "../VulkanDebugger.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Sampler::Sampler(const SamplerCreateInfo samplerCreateInfo)
        : applyBilinearFiltering(samplerCreateInfo.applyBilinearFiltering), samplerAddressMode(samplerCreateInfo.samplerAddressMode), minLod(samplerCreateInfo.minLod), maxLod(samplerCreateInfo.maxLod), maxAnisotropy(samplerCreateInfo.maxAnisotropy)
    {
        // Get the sampler filter based on whether bilinear filtering is enabled
        VkFilter samplerFilter = applyBilinearFiltering ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

        // Set up the sampler creation info
        VkSamplerCreateInfo vkSamplerCreateInfo{};
        vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vkSamplerCreateInfo.minFilter = samplerFilter;
        vkSamplerCreateInfo.magFilter = samplerFilter;
        vkSamplerCreateInfo.addressModeU = samplerAddressMode;
        vkSamplerCreateInfo.addressModeV = samplerAddressMode;
        vkSamplerCreateInfo.addressModeW = samplerAddressMode;
        vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        vkSamplerCreateInfo.compareEnable = VK_FALSE;
        vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        vkSamplerCreateInfo.mipLodBias = 0.0f;
        vkSamplerCreateInfo.minLod = minLod;
        vkSamplerCreateInfo.maxLod = maxLod;
        vkSamplerCreateInfo.anisotropyEnable = maxAnisotropy > 0.0f;
        vkSamplerCreateInfo.maxAnisotropy = maxAnisotropy;

        // Create the Vulkan sampler
        VulkanDebugger::CheckResults(
                vkCreateSampler(VulkanCore::GetLogicalDevice(), &vkSamplerCreateInfo, nullptr, &vkSampler),
                "Failed to create sampler with a LOD of [" + std::to_string(minLod) + "," + std::to_string(maxLod) + "] and [" + std::to_string(maxAnisotropy) + "] max anisotropy"
        );
    }

    std::unique_ptr<Sampler> Sampler::Create(const SamplerCreateInfo samplerCreateInfo)
    {
        return std::make_unique<Sampler>(samplerCreateInfo);
    }
    /* --- DESTRUCTOR --- */

    void Sampler::Destroy()
    {
        if (vkSampler == VK_NULL_HANDLE) return;

        // Destroy the Vulkan sampler
        vkDestroySampler(VulkanCore::GetLogicalDevice(), this->vkSampler, nullptr);

        vkSampler = VK_NULL_HANDLE;
    }
}