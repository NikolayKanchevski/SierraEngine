//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "../VulkanTypes.h"

namespace Sierra::Rendering
{
    struct SamplerCreateInfo
    {
        float minLod = 0.0f;
        float maxLod = 13.0f;
        bool enableAnisotropy = true;
        bool applyBilinearFiltering = true;
        SamplerAddressMode addressMode = SamplerAddressMode::REPEAT;
        SamplerBorderColor borderColor = SamplerBorderColor::FLOAT_OPAQUE_BLACK;
        SamplerCompareOp compareOp = SamplerCompareOp::ALWAYS;
    };

    /// @brief An abstraction for the VkSampler object.
    class Sampler
    {
    public:
        /* --- PUBLIC FIELDS --- */
        static const UniquePtr<Sampler> Default;

        /* --- POLLING METHODS --- */
        static void Initialize();
        static void Shutdown();

        /* --- CONSTRUCTORS --- */
        explicit Sampler(const SamplerCreateInfo &createInfo);
        static UniquePtr<Sampler> Create(const SamplerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkSampler GetVulkanSampler() const { return vkSampler; };
        [[nodiscard]] inline float GetMinLod() const { return createInfo.minLod; }
        [[nodiscard]] inline float GetMaxLod() const { return createInfo.maxLod; }
        [[nodiscard]] inline bool IsAnisotropyEnabled() const { return createInfo.enableAnisotropy; }
        [[nodiscard]] inline bool IsBilinearFilteringApplied() const { return createInfo.applyBilinearFiltering; }
        [[nodiscard]] inline SamplerAddressMode GetAddressMode() const { return createInfo.addressMode; }
        [[nodiscard]] inline SamplerBorderColor GetBorderColor() const { return createInfo.borderColor; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Sampler);

    private:
        VkSampler vkSampler = VK_NULL_HANDLE;
        SamplerCreateInfo createInfo;

    };

}