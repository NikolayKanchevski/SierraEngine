//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "../Types.h"

namespace Sierra::Rendering
{
    struct SamplerCreateInfo
    {
        float maxLod = 0.0f;
        bool applyBilinearFiltering = true;
        bool enableAnisotropy = true;
        SamplerBorderColor borderColor = SamplerBorderColor::FLOAT_OPAQUE_BLACK;
        SamplerCompareOp compareOp = SamplerCompareOp::ALWAYS;
    };

    /// @brief An abstraction for the VkSampler object.
    class Sampler
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Sampler(const SamplerCreateInfo &createInfo);
        static UniquePtr<Sampler> Create(const SamplerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkSampler GetVulkanSampler() const { return vkSampler; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Sampler);

    private:
        VkSampler vkSampler = VK_NULL_HANDLE;

    };

}