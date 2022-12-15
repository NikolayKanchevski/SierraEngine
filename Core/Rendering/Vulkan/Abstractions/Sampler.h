//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct SamplerCreateInfo
    {
        float minLod = 0.0f;
        float maxLod = 13.0f;
        float maxAnisotropy = 0.0f;
        bool applyBilinearFiltering = true;
        SamplerAddressMode samplerAddressMode = SamplerAddressMode::REPEAT;
    };

    /// @brief An abstraction for the VkSampler object.
    class Sampler
    {
    public:
        /* --- CONSTRUCTORS --- */
        Sampler(const SamplerCreateInfo &samplerCreateInfo);
        static std::unique_ptr<Sampler> Create(SamplerCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkSampler GetVulkanSampler() const { return this->vkSampler; };
        [[nodiscard]] inline float GetMinLod() const { return this->createInfo.minLod; }
        [[nodiscard]] inline float GetMaxLod() const { return this->createInfo.maxLod; }
        [[nodiscard]] inline float GetMaxAnisotropy() const { return this->createInfo.maxAnisotropy; }
        [[nodiscard]] inline bool IsBilinearFilteringApplied() const { return this->createInfo.applyBilinearFiltering; }
        [[nodiscard]] inline SamplerAddressMode GetAddressMode() const { return this->samplerAddressMode; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Sampler(const Sampler &) = delete;
        Sampler &operator=(const Sampler &) = delete;

    private:
        VkSampler vkSampler = VK_NULL_HANDLE;
        SamplerCreateInfo createInfo;
        SamplerAddressMode samplerAddressMode = SamplerAddressMode::REPEAT;

    };

}