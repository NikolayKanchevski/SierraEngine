//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class Sampler
    {
    public:
        /* --- CONSTRUCTORS --- */
        Sampler(bool isBilinearFilteringApplied, VkSamplerAddressMode givenSamplerAddressMode, float givenMinLod, float givenMaxLod, float givenMaxAnisotropy);

        class Builder
        {
        public:
            Builder &SetMaxAnisotropy(float givenMaxAnisotropy);

            Builder &SetAddressMode(VkSamplerAddressMode givenAddressMode);

            Builder &SetLod(glm::vec2 givenLod);

            Builder &ApplyBilinearFiltering(bool isApplied);

            [[nodiscard]] std::unique_ptr<Sampler> Build() const;

        private:
            float minLod;
            float maxLod = 13.0f;
            float maxAnisotropy = 1.0f;
            bool applyBilinearFiltering = true;
            VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkSampler GetVulkanSampler() const { return this->vkSampler; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        Sampler(const Sampler &) = delete;
        Sampler &operator=(const Sampler &) = delete;

    private:
        VkSampler vkSampler;

    };

}