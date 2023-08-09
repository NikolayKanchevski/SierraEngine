//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "Image.h"
#include "Buffer.h"

namespace Sierra::Rendering
{
    struct CommandBufferCreateInfo
    {
        CommandBufferUsage usage = CommandBufferUsage::NONE;
    };

    class CommandBuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CommandBuffer([[maybe_unused]] const CommandBufferCreateInfo &createInfo);
        static UniquePtr<CommandBuffer> Create([[maybe_unused]] const CommandBufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin();
        void End();
        void SetViewport(uint32 width, uint32 height);
        void SetScissor(uint32 width, uint32 height, int32 xOffset = 0, int32 yOffset = 0);
        void SetViewportAndScissor(uint32 width, uint32 height, int32 xOffset = 0, int32 yOffset = 0);
        void CopyBufferToBuffer(const UniquePtr<Buffer> &srcBuffer, const UniquePtr<Buffer> &dstBuffer, uint64 size = 0, uint64 srcOffset = 0, uint64 dstOffset = 0);
        void CopyBufferToImage(const UniquePtr<Buffer> &buffer, const UniquePtr<Image> &image, const Vector3 &offsets = {0, 0, 0 });
        void BlitImage(const UniquePtr<Image> &image, const Vector2 &srcOffset, const Vector2 &dstOffset, uint32 srcMipLevel);
        void SynchronizeBufferUsage(const UniquePtr<Buffer> &buffer, VkPipelineStageFlags srcStage, VkAccessFlagBits srcAccess, VkPipelineStageFlags dstStage, VkAccessFlagBits dstAccess);
        void SynchronizeImageUsage(const UniquePtr<Image> &image, VkPipelineStageFlags srcStage, VkAccessFlagBits srcAccess, VkPipelineStageFlags dstStage, VkAccessFlagBits dstAccess, ImageLayout newLayout = ImageLayout::UNDEFINED);
        void SynchronizeImageLevelUsage(const UniquePtr<Image> &image, VkPipelineStageFlags srcStage, VkAccessFlagBits srcAccess, VkPipelineStageFlags dstStage, VkAccessFlagBits dstAccess, uint32 targetLevel, ImageLayout newLayout = ImageLayout::UNDEFINED);
        void DestroyBufferAfterExecution(UniquePtr<Buffer> &buffer);
        void PushSubmissionCompletionCallback(const std::function<void()> &Callback);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsDirty() const { return isDirty; }
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return vkCommandBuffer; };

        /* --- DESTRUCTOR --- */
        void Destroy() const;
        DELETE_COPY(CommandBuffer);

    private:
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        CommandBufferUsage usage;
        bool isDirty = false;

        // Resources used upon submission completion
        void OnSubmissionCompletion();
        std::vector<UniquePtr<Buffer>> buffersToDestroy;
        std::vector<std::function<void()>> submissionCallbacks;
        friend class Device;
    };

}