//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "../Types.h"

namespace Sierra::Rendering
{

    struct ImageCreateInfo
    {
        uint32 width = 0;
        uint32 height = 0;

        ImageType imageType = ImageType::TEXTURE;
        ImageFormat format = ImageFormat::UNDEFINED;

        uint32 layerCount = 1;
        uint32 mipLevels = 1;

        ImageTiling imageTiling = ImageTiling::OPTIMAL;
        Sampling sampling = Sampling::MSAAx1;

        ImageUsage usage = ImageUsage::UNDEFINED;
        ImageCreateFlags createFlags = ImageCreateFlags::UNDEFINED;
    };

    struct SwapchainImageCreateInfo
    {
        VkImage image = VK_NULL_HANDLE;
        uint32 width = 0;
        uint32 height = 0;

        ImageFormat format = ImageFormat::UNDEFINED;
        Sampling sampling = Sampling::MSAAx1;
    };

    /// @brief Abstraction to simplify the process of working with Vulkan images and image views.
    class Image
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Image(const ImageCreateInfo &createInfo);
        static UniquePtr<Image> Create(const ImageCreateInfo &createInfo);

        // Only to be used for swapchain images!
        explicit Image(const SwapchainImageCreateInfo &createInfo);
        static UniquePtr<Image> CreateSwapchainImage(const SwapchainImageCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return width; };
        [[nodiscard]] inline uint32 GetHeight() const { return height; };

        [[nodiscard]] inline uint32 GetMipLevels() const { return mipLevels; };
        [[nodiscard]] inline uint32 GetLayerCount() const { return layerCount; };

        [[nodiscard]] inline ImageFormat GetFormat() const { return format; };
        [[nodiscard]] inline ImageUsage GetUsage() const { return usage; };
        [[nodiscard]] inline Sampling GetSampling() const { return sampling; };
        [[nodiscard]] inline ImageLayout GetLayout() const { return layout; };

        [[nodiscard]] inline ImageAspectFlags GetAspectFlags() const { return aspectFlags; };
        [[nodiscard]] [[maybe_unused]] inline bool IsSwapchainImage() const { return swapchainImage; }

        [[nodiscard]] inline VkImage GetVulkanImage() const { return vkImage; };
        [[nodiscard]] inline VkImageView GetVulkanImageView() const{ return vkImageView; }
        [[nodiscard]] [[maybe_unused]] inline VmaAllocation GetMemory() const { return vmaImageAllocation; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Image);

    private:
        uint32 width = 0;
        uint32 height = 0;

        uint32 mipLevels = 1;
        uint32 layerCount = 1;

        ImageUsage usage;
        ImageFormat format;
        Sampling sampling;
        ImageAspectFlags aspectFlags = ImageAspectFlags::UNDEFINED;
        ImageLayout layout = ImageLayout::UNDEFINED;

        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VmaAllocation vmaImageAllocation = nullptr;

        bool swapchainImage = false;
        void CreateImageView(ImageCreateFlags createFlags = ImageCreateFlags::UNDEFINED);
    };

}