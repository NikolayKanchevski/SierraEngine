//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <glm/vec3.hpp>
#include <memory>
#include <cstdint>
#include "../../../Debugger.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct ImageCreateInfo
    {
        glm::vec3 dimensions { 0, 0, 1 };
        VkFormat format = VK_FORMAT_UNDEFINED;
        uint32_t mipLevels = 1;

        VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
        VkSampleCountFlagBits sampling = VK_SAMPLE_COUNT_1_BIT;

        VkImageUsageFlags usageFlags = 0;
        VkMemoryPropertyFlags memoryFlags = 0;
    };

    struct SwapchainImageCreateInfo
    {
        VkImage image = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits sampling = VK_SAMPLE_COUNT_1_BIT;
        glm::vec3 dimensions = { 0, 0, 0 };
    };

    /// @brief Abstraction to simplify the process of working with Vulkan images and image views.
    class Image
    {
    public:
        /* --- CONSTRUCTORS --- */
        Image(ImageCreateInfo imageCreateInfo);
        [[nodiscard]] static std::unique_ptr<Image> Create(ImageCreateInfo imageCreateInfo);

        // Only to be used for swapchain images!
        Image(SwapchainImageCreateInfo swapchainImageCreateInfo);
        static std::unique_ptr<Image> CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo);

        /* --- SETTER METHODS --- */
        void CreateImageView(VkImageAspectFlags givenAspectFlags);
        void TransitionLayout(VkImageLayout newLayout);
        void DestroyImageView();

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
        { if (!imageViewGenerated) { ASSERT_WARNING("Image view not generated. Returning null"); return NULL; } return this->vkImageView; };

        [[nodiscard]] inline VkDeviceMemory GetVulkanMemory() const
        { return this->vkImageMemory; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        inline ~Image() { Destroy(); }
        Image(const Image &) = delete;
        Image &operator=(const Image &) = delete;

    private:
        glm::vec3 dimensions { 0, 0, 1 };

        uint32_t mipLevels = 1;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits sampling;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;

        bool imageViewGenerated = false;
        bool swapchainImage = false;
    };

}