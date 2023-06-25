//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct Dimensions
    {
        uint width = 0;
        uint height = 0;
        uint depth = 1;
    };

    struct ImageCreateInfo
    {
        Dimensions dimensions{};
        ImageType imageType = ImageType::TEXTURE;
        ImageFormat format = ImageFormat::UNDEFINED;

        uint layerCount = 1;
        bool generateMipMaps = false;

        ImageTiling imageTiling = ImageTiling::OPTIMAL;
        Sampling sampling = Sampling::MSAAx1;

        ImageUsage usage = ImageUsage::UNDEFINED;
        ImageCreateFlags createFlags = ImageCreateFlags::UNDEFINED;
    };

    struct SwapchainImageCreateInfo
    {
        VkImage image = VK_NULL_HANDLE;
        ImageFormat format = ImageFormat::UNDEFINED;
        Sampling sampling = Sampling::MSAAx1;
        Dimensions dimensions{};
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
        static UniquePtr<Image> CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo);

        /* --- SETTER METHODS --- */
        bool GenerateMipMaps();
        void TransitionLayout(ImageLayout newLayout);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Dimensions GetDimensions() const
        { return dimensions; };

        [[nodiscard]] inline uint GetWidth() const
        { return dimensions.width; };

        [[nodiscard]] inline uint GetHeight() const
        { return dimensions.height; };

        [[nodiscard]] inline uint GetDepth() const
        { return dimensions.depth; };

        [[nodiscard]] inline uint GetMipMapLevels() const
        { return mipLevels; };

        [[nodiscard]] inline ImageFormat GetFormat() const
        { return format; };

        [[nodiscard]] inline ImageUsage GetUsage() const
        { return usage; };

        [[nodiscard]] inline Sampling GetSampling() const
        { return sampling; };

        [[nodiscard]] inline ImageLayout GetLayout() const
        { return layout; };

        [[nodiscard]] inline uint GetLayerCount() const
        { return layerCount; };

        [[nodiscard]] inline ImageAspectFlags GetAspectFlags() const
        { return aspectFlags; };

        [[nodiscard]] [[maybe_unused]] inline bool IsSwapchainImage() const
        { return swapchainImage; }

        [[nodiscard]] inline VkImage GetVulkanImage() const
        { return vkImage; };

        [[nodiscard]] inline VkImageView GetVulkanImageView() const
        { return vkImageView; }

        [[nodiscard]] [[maybe_unused]] inline VmaAllocation GetMemory() const
        { return vmaImageAllocation; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Image);

    private:
        Dimensions dimensions = {};

        uint mipLevels = 1;
        uint layerCount = 1;

        ImageUsage usage;
        ImageFormat format;
        Sampling sampling;
        ImageType imageType;
        ImageAspectFlags aspectFlags = ImageAspectFlags::UNDEFINED;
        ImageLayout layout = ImageLayout::UNDEFINED;
        ImageCreateFlags flags;

        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VmaAllocation vmaImageAllocation = nullptr;

        bool swapchainImage = false;

        void CreateImageView();
        friend class CommandBuffer;
    };

}