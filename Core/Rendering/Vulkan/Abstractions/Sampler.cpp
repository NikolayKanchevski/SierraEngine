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

    Sampler::Sampler(bool isBilinearFilteringApplied, VkSamplerAddressMode givenSamplerAddressMode, float givenMinLod, float givenMaxLod, float givenMaxAnisotropy)
    {
        // Get the sampler filter based on whether bilinear filtering is enabled
        VkFilter samplerFilter = isBilinearFilteringApplied ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

        // Set up the sampler creation info
        VkSamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.minFilter = samplerFilter;
        samplerCreateInfo.magFilter = samplerFilter;
        samplerCreateInfo.addressModeU = givenSamplerAddressMode;
        samplerCreateInfo.addressModeV = givenSamplerAddressMode;
        samplerCreateInfo.addressModeW = givenSamplerAddressMode;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = givenMinLod;
        samplerCreateInfo.maxLod = givenMaxLod;
        samplerCreateInfo.anisotropyEnable = givenMaxAnisotropy > 0.0f;
        samplerCreateInfo.maxAnisotropy = givenMaxAnisotropy > 0.0f ? givenMaxAnisotropy : 0.0f;

        // Create the Vulkan sampler
        VulkanDebugger::CheckResults(
            vkCreateSampler(VulkanCore::GetLogicalDevice(), &samplerCreateInfo, nullptr, &vkSampler),
            "Failed to create sampler with a LOD of [" + std::to_string(givenMinLod) + "," + std::to_string(givenMaxLod) + "] and [" + std::to_string(givenMaxAnisotropy) + "] max anisotropy"
        );
    }

    Sampler::Builder &Sampler::Builder::SetMaxAnisotropy(float givenMaxAnisotropy)
    {
        // Check if sampler anisotropy is supported by the GPU
        if (VulkanCore::GetPhysicalDeviceFeatures().samplerAnisotropy)
        {
            // Clamp the anisotropy between 0.0 and 1.0 and multiply it by the maximum supported anisotropy
            givenMaxAnisotropy = Math::Clamp(givenMaxAnisotropy, 0.0f, 1.0f);
            this->maxAnisotropy = (givenMaxAnisotropy / 1.0f) * VulkanCore::GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
        }
        else
        {
            VulkanDebugger::ThrowWarning("Sampler anisotropy is requested but not supported by the GPU. The feature has automatically been disabled");
        }

        return *this;
    }

    Sampler::Builder &Sampler::Builder::SetAddressMode(VkSamplerAddressMode givenAddressMode)
    {
        // Save the provided address mode
        samplerAddressMode = givenAddressMode;
        return *this;
    }

    Sampler::Builder &Sampler::Builder::SetLod(glm::vec2 givenLod)
    {
        // Save the provided LOD values
        minLod = givenLod.x;
        maxAnisotropy = givenLod.y;
        return *this;
    }

    Sampler::Builder &Sampler::Builder::ApplyBilinearFiltering(bool isApplied)
    {
        // Save the bilinear filtering preference
        applyBilinearFiltering = isApplied;
        return *this;
    }

    std::unique_ptr<Sampler> Sampler::Builder::Build() const
    {
        return std::make_unique<Sampler>(applyBilinearFiltering, samplerAddressMode, minLod, maxLod, maxAnisotropy);
    }

    /* --- DESTRUCTOR --- */

    Sampler::~Sampler()
    {
        // Destroy the Vulkan sampler
        vkDestroySampler(VulkanCore::GetLogicalDevice(), this->vkSampler, nullptr);
    }
}