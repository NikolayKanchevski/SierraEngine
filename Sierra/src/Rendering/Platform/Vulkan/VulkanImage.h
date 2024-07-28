//
// Created by Nikolay Kanchevski on 7.12.23.
//

#pragma once

#include "../../Image.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanImage final : public Image, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanImage(const VulkanDevice &device, const ImageCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; }

        [[nodiscard]] uint32 GetWidth() const override { return width; }
        [[nodiscard]] uint32 GetHeight() const override { return height; }
        [[nodiscard]] uint32 GetDepth() const override { return depth; }
        [[nodiscard]] ImageFormat GetFormat() const override { return format; }

        [[nodiscard]] uint32 GetLevelCount() const override { return levelCount; }
        [[nodiscard]] uint32 GetLayerCount() const override { return layerCount; }
        [[nodiscard]] ImageSampling GetSampling() const override { return sampling; }

        [[nodiscard]] VkImage GetVulkanImage() const { return image; }
        [[nodiscard]] VkImageView GetVulkanImageView() const { return imageView; }
        [[nodiscard]] VkImageAspectFlags GetVulkanAspectFlags() const { return aspectFlags; }
        [[nodiscard]] VkImageUsageFlags GetVulkanUsageFlags() const { return usageFlags; }

        /* --- DESTRUCTOR --- */
        ~VulkanImage() override;

        /* --- CONVERSIONS --- */
        static VkImageType ImageTypeToVkImageType(ImageType type);
        static VkFormat ImageFormatToVkFormat(ImageFormat format);
        static VkSampleCountFlagBits ImageSamplingToVkSampleCountFlags(ImageSampling sampling);
        static VkImageUsageFlags ImageUsageToVkImageUsageFlags(ImageUsage usage);
        static VmaMemoryUsage ImageMemoryLocationToVmaMemoryUsage(ImageMemoryLocation memoryLocation);
        static VkImageViewType ImageTypeToVkImageViewType(ImageType type, uint32 layerCount);

    private:
        const VulkanDevice &device;

        uint32 width = 0;
        uint32 height = 0;
        uint32 depth = 0;
        ImageFormat format = ImageFormat::Undefined;

        uint32 levelCount = 1;
        uint32 layerCount = 1;
        ImageSampling sampling = ImageSampling::x1;

        std::string name;
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
        VulkanImage(const VulkanDevice &device, const SwapchainImageCreateInfo &createInfo);
        [[nodiscard]] static ImageFormat SwapchainVkFormatToImageFormat(VkFormat format);

    };

}
