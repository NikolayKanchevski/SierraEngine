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
        [[nodiscard]] inline VkImage GetVulkanImage() const { return image; }
        [[nodiscard]] inline VkImageView GetVulkanImageView() const { return imageView; }
        [[nodiscard]] inline VkImageUsageFlags GetVulkanUsageFlags() const { return usageFlags; }

        /* --- DESTRUCTOR --- */
        ~VulkanImage() override;

        /* --- CONVERSIONS --- */
        static VkFormat ImageFormatToVkFormat(ImageFormat format);
        static VkSampleCountFlagBits ImageSamplingToVkSampleCountFlags(ImageSampling sampling);
        static VkImageUsageFlags ImageUsageToVkImageUsageFlags(ImageUsage usage);
        static VmaMemoryUsage ImageMemoryLocationToVmaMemoryUsage(ImageMemoryLocation memoryLocation);

    private:
        const VulkanDevice &device;

        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;

        VkImageUsageFlags usageFlags = 0;
        VmaAllocation allocation = nullptr;

        friend class VulkanSwapchain;
        struct SwapchainImageCreateInfo
        {
            const std::string &name = "Swapchain Image";
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
