//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <glm/vec3.hpp>
#include <memory>
#include "../VulkanDebugger.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /// Abstraction to simplify the process of working with Vulkan images and image views.
    class Image
    {
    public:
        /* --- CONSTRUCTORS --- */
        Image(glm::vec3 givenDimensions, uint32_t givenMipLevels, VkSampleCountFlagBits givenSampling, VkFormat givenFormat, VkImageTiling imageTiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
        inline Image(VkImage givenVkImage, const VkFormat givenFormat, const VkSampleCountFlagBits givenSampling, const glm::vec3 givenDimensions, const uint32_t givenMipLevels = 1, const VkImageLayout givenLayout = VK_IMAGE_LAYOUT_UNDEFINED)
            : vkImage(givenVkImage), format(givenFormat), sampling(givenSampling), dimensions(givenDimensions), mipLevels(givenMipLevels), layout(givenLayout) { }


        class Builder
        {
        public:
            Builder& SetDimensions(glm::vec3 givenDimensions);
            Builder& SetWidth(uint32_t givenWidth);
            Builder& SetHeight(uint32_t givenHeight);
            Builder& SetDepth(uint32_t givenDepth);

            Builder& SetMipLevels(uint32_t givenMipLevels);
            Builder& SetFormat(VkFormat givenFormat);
            Builder& SetUsageFlags(VkImageUsageFlags givenUsageFlags);
            Builder& SetMemoryFlags(VkMemoryPropertyFlags givenMemoryFlags);
            Builder& SetImageTiling(VkImageTiling givenImageTiling);
            Builder& SetSampling(VkSampleCountFlagBits givenSampling);
            [[nodiscard]] std::unique_ptr<Image> Build() const;

        private:
            glm::vec3 dimensions { 0, 0, 1 };
            uint32_t mipLevels = 1;

            VkFormat format;
            VkImageUsageFlags usageFlags;
            VkMemoryPropertyFlags memoryFlags;

            VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
            VkSampleCountFlagBits sampling = VK_SAMPLE_COUNT_1_BIT;
        };

        /* --- SETTER METHODS --- */
        void CreateImageView(VkImageAspectFlags givenAspectFlags);
        void TransitionLayout(VkImageLayout newLayout);

        void DestroyVulkanImage();
        void DestroyVulkanImageView();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline glm::vec3 GetDimensions() const
        { return this->dimensions; };

        [[nodiscard]] inline float GetWidth() const
        { return this->dimensions.x; };

        [[nodiscard]] inline float GetHeight() const
        { return this->dimensions.y; };

        [[nodiscard]] inline float GetDepth() const
        { return this->dimensions.z; };

        [[nodiscard]] inline uint32_t GetMipLevels() const
        { return this->mipLevels; };

        [[nodiscard]] inline VkFormat GetFormat() const
        { return this->format; };

        [[nodiscard]] inline VkSampleCountFlagBits GetSampling() const
        { return this->sampling; };

        [[nodiscard]] inline VkImageLayout GetLayout() const
        { return this->layout; };

        [[nodiscard]] inline VkImage GetVulkanImage() const
        { return this->vkImage; };

        [[nodiscard]] inline VkImageView GetVulkanImageView() const
        { if (!imageViewGenerated) { VulkanDebugger::ThrowWarning("Image view not generated. Returning null"); return NULL; } return this->vkImageView; };

        [[nodiscard]] inline VkDeviceMemory GetVulkanMemory() const
        { return this->vkImageMemory; };

        /* --- DESTRUCTOR --- */
        ~Image();
        Image(const Image &) = delete;
        Image &operator=(const Image &) = delete;

    private:

        glm::vec3 dimensions;

        uint32_t mipLevels;
        VkFormat format;
        VkSampleCountFlagBits sampling;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage vkImage;
        VkImageView vkImageView;
        VkDeviceMemory vkImageMemory;
        bool imageViewGenerated;
    };

}