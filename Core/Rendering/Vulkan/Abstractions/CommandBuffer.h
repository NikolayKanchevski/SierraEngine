//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "Image.h"
#include "Buffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct CommandBufferCreateInfo
    {

    };

    class CommandBuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        CommandBuffer([[maybe_unused]] const UniquePtr<CommandBufferCreateInfo> &createInfo);
        static UniquePtr<CommandBuffer> Create([[maybe_unused]] const UniquePtr<CommandBufferCreateInfo> &createInfo);

        /* --- POLLING METHODS --- */
        void Begin(CommandBufferUsage usage = CommandBufferUsage::NONE) const;
        void End() const;
        void Reset() const;
        void Free() const;
        void TransitionImageLayout(Image *image, ImageLayout newLayout);
        void TransitionImageLayout(const UniquePtr<Image> &image, ImageLayout newLayout);
        void TransitionImageLayouts(const std::vector<ReferenceWrapper<UniquePtr<Image>>> &images, ImageLayout newLayout);
        void SetViewport(uint width, uint height) const;
        void SetScissor(uint width, uint height, int xOffset = 0, int yOffset = 0) const;
        void SetViewportAndScissor(uint width, uint height, int xOffset = 0, int yOffset = 0) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return vkCommandBuffer; };

        /* --- OPERATIONS --- */
        DELETE_COPY(CommandBuffer);

    private:
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

        struct ImageLayoutPair
        {
            ImageLayout initialLayout = ImageLayout::UNDEFINED;
            bool firstTime = false;
        };

        // [Pointer to image | Initial image layout before first TransitionLayout() | First time transitioning]
        std::unordered_map<Image*, ImageLayoutPair> initialImageLayouts;
    };

}