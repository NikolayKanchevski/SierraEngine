//
// Created by Nikolay Kanchevski on 7.12.23.
//

#pragma once

#include "../Image.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkImageType ImageTypeToVkImageType(ImageType type) noexcept;
    [[nodiscard]] SIERRA_API VkImageViewType ImageTypeToVkImageViewType(ImageType type, uint32 layerCount) noexcept;
    [[nodiscard]] SIERRA_API VkFormat ImageFormatToVkFormat(ImageFormat format) noexcept;
    [[nodiscard]] SIERRA_API VkImageUsageFlags ImageUsageToVkImageUsageFlags(ImageUsage usage) noexcept;
    [[nodiscard]] SIERRA_API VkSampleCountFlagBits ImageSamplingToVkSampleCountFlags(ImageSampling sampling) noexcept;
    [[nodiscard]] SIERRA_API VkComponentSwizzle ImageComponentSwizzleToVkComponentSwizzle(ImageComponentSwizzle componentSwizzle) noexcept;
    [[nodiscard]] SIERRA_API VmaMemoryUsage ImageMemoryLocationToVmaMemoryUsage(ImageMemoryLocation memoryLocation) noexcept;

    class SIERRA_API VulkanImage final : public Image, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanImage(const VulkanDevice& device, const ImageCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] uint32 GetWidth() const noexcept override { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return height; }
        [[nodiscard]] uint32 GetDepth() const noexcept override { return depth; }
        [[nodiscard]] ImageFormat GetFormat() const noexcept override { return format; }

        [[nodiscard]] uint32 GetLevelCount() const noexcept override { return levelCount; }
        [[nodiscard]] uint32 GetLayerCount() const noexcept override { return layerCount; }
        [[nodiscard]] ImageSampling GetSampling() const noexcept override { return sampling; }

        [[nodiscard]] VkImage GetVulkanImage() const noexcept { return image; }
        [[nodiscard]] VkImageView GetVulkanImageView() const noexcept { return imageView; }
        [[nodiscard]] VkImageAspectFlags GetVulkanAspectFlags() const noexcept { return aspectFlags; }
        [[nodiscard]] VkImageUsageFlags GetVulkanUsageFlags() const noexcept { return usageFlags; }

        /* --- COPY SEMANTICS --- */
        VulkanImage(const VulkanImage&) = delete;
        VulkanImage& operator=(const VulkanImage&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanImage(VulkanImage&&) = delete;
        VulkanImage& operator=(VulkanImage&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanImage() noexcept override;

    private:
        const VulkanDevice& device;
        const std::string name;

        const uint32 width = 0;
        const uint32 height = 0;
        const uint32 depth = 0;
        const ImageFormat format = ImageFormat::Undefined;

        const uint32 levelCount = 1;
        const uint32 layerCount = 1;
        const ImageSampling sampling = ImageSampling::x1;

        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;

        VkImageUsageFlags usageFlags = 0;
        VkImageAspectFlags aspectFlags = 0;
        VmaAllocation allocation = nullptr;

        friend class VulkanSwapchain;
        struct SwapchainImageCreateInfo
        {
            std::string_view name = "Swapchain Image";
            VkImage image = VK_NULL_HANDLE;

            uint32 width = 0;
            uint32 height = 0;
            VkFormat format = VK_FORMAT_UNDEFINED;
        };

        bool swapchainImage = false;
        VulkanImage(const VulkanDevice& device, const SwapchainImageCreateInfo& createInfo);

    };

}
