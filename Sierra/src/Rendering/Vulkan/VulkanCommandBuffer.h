//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../CommandBuffer.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanResourceTable.h"
#include "VulkanBuffer.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkAccessFlags BufferCommandUsageToVkAccessFlags(BufferCommandUsage bufferCommandUsage) noexcept;
    [[nodiscard]] SIERRA_API VkPipelineStageFlags BufferCommandUsageToVkPipelineStageFlags(BufferCommandUsage bufferCommandUsage) noexcept;
    [[nodiscard]] SIERRA_API VkImageLayout ImageCommandUsageToVkLayout(ImageCommandUsage imageCommandUsage) noexcept;
    [[nodiscard]] SIERRA_API VkAccessFlags ImageCommandUsageToVkAccessFlags(ImageCommandUsage imageCommandUsage) noexcept;
    [[nodiscard]] SIERRA_API VkPipelineStageFlags ImageCommandUsageToVkPipelineStageFlags(ImageCommandUsage imageCommandUsage) noexcept;

    class SIERRA_API VulkanCommandBuffer final : public CommandBuffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanCommandBuffer(const VulkanQueue& queue, const CommandBufferCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        void SynchronizeBufferUsage(const Buffer& buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, size offset, size memorySize) override;
        void SynchronizeImageUsage(const Image& image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseLevel, uint32 levelCount, uint32 baseLayer, uint32 layerCount) override;

        void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, size sourceOffset, size destinationOffset, size memorySize) override;
        void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, uint32 level, uint32 layer, size sourceOffset, Vector3UInt destinationPixelOffset, Vector3UInt pixelRange) override;
        void GenerateMipMapsForImage(const Image& image) override;

        void BindResourceTable(const ResourceTable& resourceTable) override;
        void PushConstants(const void* memory, size sourceOffset, size memorySize) override;

        void BeginRenderPass(const RenderPass& renderPass, std::span<const RenderPassBeginAttachment> attachments) override;
        void BeginNextSubpass(const RenderPass& renderPass) override;
        void EndRenderPass(const RenderPass& renderPass) override;

        void BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) override;
        void EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) override;

        void BindVertexBuffer(const Buffer& vertexBuffer, size offset) override;
        void BindIndexBuffer(const Buffer& indexBuffer, size offset) override;

        void SetScissor(Vector4UInt scissor) override;
        void Draw(uint32 vertexCount, size vertexOffset) override;
        void DrawIndexed(uint32 indexCount, size indexOffset, size vertexOffset) override;

        void BeginComputePipeline(const ComputePipeline& computePipeline) override;
        void EndComputePipeline(const ComputePipeline& computePipeline) override;

        void Dispatch(Vector3UInt workGroupSize) override;

        void BeginDebugRegion(std::string_view regionName, Color32 color) override;
        void InsertDebugMarker(std::string_view markerName, Color32 color) override;
        void EndDebugRegion() override;

        std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) override;
        std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] VkCommandBuffer GetVulkanCommandBuffer() const noexcept { return commandBuffer; }
        [[nodiscard]] uint32 GetQueueFamily() const noexcept { return queue.GetFamily(); }

        [[nodiscard]] QueueOperations GetOperations() const noexcept { return operations; }
        [[nodiscard]] uint64 GetCompletionSemaphoreSignalValue() const noexcept { return completionSemaphoreSignalValue; }

        /* --- DESTRUCTOR --- */
        ~VulkanCommandBuffer() noexcept override;

    private:
        const VulkanQueue& queue;
        const std::string name;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        QueueOperations operations = QueueOperations::None;
        uint64 completionSemaphoreSignalValue = 0;

        bool debugRegionBegan = false;
        const VulkanResourceTable* currentResourceTable = nullptr;

        uint32 currentSubpass = 0;
        const VulkanRenderPass* currentRenderPass = nullptr;

        const VulkanGraphicsPipeline* currentGraphicsPipeline = nullptr;
        const VulkanComputePipeline* currentComputePipeline = nullptr;

        size initialVertexBufferOffset = 0;
        const VulkanBuffer* currentVertexBuffer = nullptr;

        size initialIndexBufferOffset = 0;
        const VulkanBuffer* currentIndexBuffer = nullptr;

        std::queue<std::unique_ptr<Buffer>> queuedBuffersForDestruction;
        std::queue<std::unique_ptr<Image>> queuedImagesForDestruction;

    };

}
