//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct SamplerCreateInfo
    {
        float minLod = 0.0f;
        float maxLod = 13.0f;
        float maxAnisotropy = 0.0f;
        bool applyBilinearFiltering = true;
        VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
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
        [[nodiscard]] inline float GetMinLod() const { return this->minLod; }
        [[nodiscard]] inline float GetMaxLod() const { return this->maxLod; }
        [[nodiscard]] inline float GetMaxAnisotropy() const { return this->maxAnisotropy; }
        [[nodiscard]] inline bool IsBilinearFilteringApplied() const { return this->applyBilinearFiltering; }
        [[nodiscard]] inline VkSamplerAddressMode GetAddressMode() const { return this->samplerAddressMode; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Sampler(const Sampler &) = delete;
        Sampler &operator=(const Sampler &) = delete;

    private:
        VkSampler vkSampler = VK_NULL_HANDLE;
        float minLod = 1.0;
        float maxLod = 13.0f;
        float maxAnisotropy = 0.0f;
        bool applyBilinearFiltering = true;
        VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    };

}