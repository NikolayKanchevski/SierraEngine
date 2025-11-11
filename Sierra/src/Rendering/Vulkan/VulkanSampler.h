//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../Sampler.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkFilter SamplerSampleModeToVkFilter(SamplerFilter sampleMode) noexcept;
    [[nodiscard]] SIERRA_API VkSamplerMipmapMode SamplerSampleModeToVkSamplerMipMapMode(SamplerFilter sampleMode) noexcept;
    [[nodiscard]] SIERRA_API VkSamplerAddressMode SamplerExtendModeToVkSamplerAddressMode(SamplerAddressMode extendMode) noexcept;
    [[nodiscard]] SIERRA_API float32 SamplerAnisotropyToFloat32(SamplerAnisotropy anisotropy) noexcept;
    [[nodiscard]] SIERRA_API VkCompareOp SamplerCompareOperationToVkCompareOp(SamplerCompareOperation compareOperation) noexcept;
    [[nodiscard]] SIERRA_API VkBorderColor SamplerBorderColorToVkBorderColor(SamplerBorderColor borderColor) noexcept;

    class SIERRA_API VulkanSampler final : public Sampler, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanSampler(const VulkanDevice& device, const SamplerCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkSampler GetVulkanSampler() const noexcept { return sampler; }

        /* --- COPY SEMANTICS --- */
        VulkanSampler(const VulkanSampler&) = delete;
        VulkanSampler& operator=(const VulkanSampler&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanSampler(VulkanSampler&&) noexcept = default;
        VulkanSampler& operator=(VulkanSampler&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanSampler() noexcept override;

    private:
        const VulkanDevice* device = nullptr;

        VkSampler sampler = VK_NULL_HANDLE;

    };

}
