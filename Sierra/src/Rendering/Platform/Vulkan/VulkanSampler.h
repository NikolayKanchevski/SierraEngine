//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../../Sampler.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanSampler final : public Sampler, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanSampler(const VulkanDevice &device, const SamplerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkSampler GetVulkanSampler() const { return sampler; }

        /* --- DESTRUCTOR --- */
        ~VulkanSampler() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkFilter SamplerSampleModeToVkFilter(SamplerFilter sampleMode);
        [[nodiscard]] static VkSamplerMipmapMode SamplerSampleModeToVkSamplerMipMapMode(SamplerFilter sampleMode);
        [[nodiscard]] static VkSamplerAddressMode SamplerExtendModeToVkSamplerAddressMode(SamplerAddressMode extendMode);
        [[nodiscard]] static float32 SamplerAnisotropyToFloat32(SamplerAnisotropy anisotropy);
        [[nodiscard]] static VkCompareOp SamplerCompareOperationToVkCompareOp(SamplerCompareOperation compareOperation);
        [[nodiscard]] static VkBorderColor SamplerBorderColorToVkBorderColor(SamplerBorderColor borderColor);

    private:
        const VulkanDevice &device;
        VkSampler sampler = VK_NULL_HANDLE;

    };

}
