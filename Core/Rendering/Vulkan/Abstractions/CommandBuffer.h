//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class CommandBuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        CommandBuffer();
        static UniquePtr<CommandBuffer> Create();

        /* --- POLLING METHODS --- */
        void Begin(VkCommandBufferUsageFlagBits flags = (VkCommandBufferUsageFlagBits) 0) const;
        void End() const;
        void Reset() const;
        void Free() const;
        void TransitionImageLayout(Image *image, ImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
        void TransitionImageLayout(const UniquePtr<Image> &image, ImageLayout newLayout, VkPipelineStageFlags lastUsageStage, VkPipelineStageFlags expectedUsageStage);
        void TransitionImageLayouts(const std::vector<ImageReference>& images, ImageLayout newLayout, VkPipelineStageFlags lastUsageStage, VkPipelineStageFlags expectedUsageStage);
        void BindVertexBuffers(const std::vector<VkBuffer> &vertexBuffers) const;
        void BindIndexBuffer(const VkBuffer &indexBuffer) const;
        void DrawIndexed(const uint indexCount) const;
        void Draw(const uint vertexCount) const;
        void Dispatch(uint xCount, uint yCount = 1, uint zCount = 1);
        void SetViewport(uint width, uint height) const;
        void SetScissor(uint width, uint height, int xOffset = 0, int yOffset = 0) const;
        void SetViewportAndScissor(uint width, uint height, int xOffset = 0, int yOffset = 0) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return vkCommandBuffer; };

        /* --- OPERATIONS --- */
        DELETE_COPY(CommandBuffer);

    private:
        VkCommandBuffer vkCommandBuffer;

        struct ImageLayoutPair
        {
            ImageLayout initialLayout = ImageLayout::UNDEFINED;
            bool firstTime = false;
        };

        // [Pointer to image | Initial image layout before first TransitionLayout() | First time transitioning]
        std::unordered_map<Image*, ImageLayoutPair> initialImageLayouts;
    };

}