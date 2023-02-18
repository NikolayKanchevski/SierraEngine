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

        ImageUsage usageFlags = ImageUsage::UNDEFINED;
        ImageCreateFlags createFlags = ImageCreateFlags::NONE;
        MemoryFlags memoryFlags = MemoryFlags::NONE;
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
        Image(const ImageCreateInfo &createInfo);
        [[nodiscard]] static UniquePtr<Image> Create(ImageCreateInfo imageCreateInfo);

        // Only to be used for swapchain images!
        Image(const SwapchainImageCreateInfo &createInfo);
        static UniquePtr<Image> CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo);

        /* --- SETTER METHODS --- */
        bool GenerateMipMaps();
        void CreateImageView(ImageAspectFlags givenAspectFlags, VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D);
        void TransitionLayout(ImageLayout newLayout);
        void DestroyImageView();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Dimensions GetDimensions() const
        { return this->dimensions; };

        [[nodiscard]] inline float GetWidth() const
        { return this->dimensions.width; };

        [[nodiscard]] inline float GetHeight() const
        { return this->dimensions.height; };

        [[nodiscard]] inline float GetDepth() const
        { return this->dimensions.depth; };

        [[nodiscard]] inline uint GetMipMapLevels() const
        { return this->mipLevels; };

        [[nodiscard]] inline ImageFormat GetFormat() const
        { return this->format; };

        [[nodiscard]] inline Sampling GetSampling() const
        { return this->sampling; };

        [[nodiscard]] inline ImageLayout GetLayout() const
        { return this->layout; };

        [[nodiscard]] inline uint GetLayerCount() const
        { return this->layerCount; };

        [[nodiscard]] inline VkImage GetVulkanImage() const
        { return this->vkImage; };

        [[nodiscard]] inline VkImageView GetVulkanImageView() const
        { ASSERT_WARNING_IF(!imageViewCreated, "Image view not generated. Returning null"); return vkImageView; }

        [[nodiscard]] inline VmaAllocation GetMemory() const
        { return this->vmaImageAllocation; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Image);

    private:
        Dimensions dimensions{};

        uint mipLevels = 1;
        uint layerCount = 1;

        ImageFormat format = ImageFormat::UNDEFINED;
        Sampling sampling;
        ImageLayout layout = ImageLayout::UNDEFINED;

        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VmaAllocation vmaImageAllocation;

        bool imageViewCreated = false;
        bool mipMapsGenerated = false;
        bool swapchainImage = false;
    };

}