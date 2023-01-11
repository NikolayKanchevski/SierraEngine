//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "../../../Debugger.h"
#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct Dimensions
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
    };

    struct ImageCreateInfo
    {
        Dimensions dimensions{};
        VkImageType imageType = VK_IMAGE_TYPE_2D;
        ImageFormat format = FORMAT_UNDEFINED;

        uint32_t layerCount = 1;

        ImageTiling imageTiling = TILING_OPTIMAL;
        Sampling sampling = Sampling::MSAAx1;

        ImageUsage usageFlags = UNDEFINED_IMAGE;
        ImageCreateFlags createFlags = IMAGE_FLAGS_NONE;
        MemoryFlags memoryFlags = MEMORY_FLAGS_NONE;
    };

    struct SwapchainImageCreateInfo
    {
        VkImage image = VK_NULL_HANDLE;
        ImageFormat format = FORMAT_UNDEFINED;
        Sampling sampling = MSAAx1;
        Dimensions dimensions{};
    };

    /// @brief Abstraction to simplify the process of working with Vulkan images and image views.
    class Image
    {
    public:
        /* --- CONSTRUCTORS --- */
        Image(const ImageCreateInfo &createInfo);
        [[nodiscard]] static std::unique_ptr<Image> Create(ImageCreateInfo imageCreateInfo);

        // Only to be used for swapchain images!
        Image(const SwapchainImageCreateInfo &createInfo);
        static std::unique_ptr<Image> CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo);

        /* --- SETTER METHODS --- */
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

        [[nodiscard]] inline uint32_t GetMipMapLevels() const
        { return this->mipMapLevels; };

        [[nodiscard]] inline ImageFormat GetFormat() const
        { return this->format; };

        [[nodiscard]] inline Sampling GetSampling() const
        { return this->sampling; };

        [[nodiscard]] inline ImageLayout GetLayout() const
        { return this->layout; };

        [[nodiscard]] inline uint32_t GetLayerCount() const
        { return this->layerCount; };

        [[nodiscard]] inline VkImage GetVulkanImage() const
        { return this->vkImage; };

        [[nodiscard]] inline VkImageView GetVulkanImageView() const
        { ASSERT_WARNING_IF(!imageViewCreated, "Image view not generated. Returning null"); return vkImageView; }

        [[nodiscard]] inline VmaAllocation GetMemory() const
        { return this->vmaImageAllocation; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        Image(const Image &) = delete;
        Image &operator=(const Image &) = delete;

    private:
        Dimensions dimensions{};

        uint32_t mipMapLevels = 1;
        uint32_t layerCount = 1;

        ImageFormat format = FORMAT_UNDEFINED;
        Sampling sampling;
        ImageLayout layout = LAYOUT_UNDEFINED;

        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VmaAllocation vmaImageAllocation;

        bool imageViewCreated = false;
        bool mipMapsGenerated = false;
        bool swapchainImage = false;
    };

}