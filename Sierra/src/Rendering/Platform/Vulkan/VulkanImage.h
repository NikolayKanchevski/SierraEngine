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
        [[nodiscard]] inline ImageSampling GetSampling() const override { return sampling; }
        [[nodiscard]] inline VkImage GetVulkanImage() const { return image; }
        [[nodiscard]] inline VkImageView GetVulkanImageView() const { return imageView; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- CONVERSIONS --- */
        static VkFormat ImageFormatToVkFormat(ImageFormat format);
        static VkSampleCountFlagBits ImageSamplingToVkSampleCountFlags(ImageSampling sampling);
        static VkImageUsageFlags ImageUsageToVkImageUsageFlags(ImageUsage usage);
        static VmaMemoryUsage ImageMemoryLocationToVmaMemoryUsage(ImageMemoryLocation memoryLocation);

    private:
        const VulkanDevice &device;

        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VmaAllocation allocation = nullptr;

        ImageSampling sampling = ImageSampling::x1;

    };

}
