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
#include "VulkanImage.h"

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

        void SynchronizeBufferUsage(const Buffer& buffer, const BufferSynchronizeInfo& synchronizeInfo) override;
        void SynchronizeImageUsage(const Image& image, const ImageSynchronizeInfo& synchronizeInfo) override;

        void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const BufferToBufferCopyInfo& copyInfo) override;
        void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const BufferToImageCopyInfo& copyInfo) override;
        void GenerateMipMapsForImage(const Image& image) override;

        void BindResourceTable(const ResourceTable& resourceTable) override;
        void PushConstants(std::span<const uint8> buffer) override;

        void BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) override;
        void BeginNextSubpass() override;
        void EndRenderPass() override;

        void BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) override;
        void EndGraphicsPipeline() override;

        void BindVertexBuffer(const Buffer& vertexBuffer, uint64 offset) override;
        void BindIndexBuffer(const Buffer& indexBuffer, uint64 offset) override;

        void SetScissor(Vector4UInt scissor) override;
        void Draw(uint32 vertexCount, uint64 vertexOffset) override;
        void DrawIndexed(uint32 indexCount, uint64 indexOffset, uint64 vertexOffset) override;

        void BeginComputePipeline(const ComputePipeline& computePipeline) override;
        void EndComputePipeline() override;

        void Dispatch(Vector3UInt workGroupSize) override;

        void BeginDebugRegion(std::string_view regionName, Color32 color) override;
        void InsertDebugMarker(std::string_view markerName, Color32 color) override;
        void EndDebugRegion() override;

        void BeginTimestampRegion(std::string_view regionName) override;
        void EndTimestampRegion() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::span<const TimestampRegion> GetTimestampRegions() const noexcept override { return timestampRegions; }

        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] VkCommandBuffer GetVulkanCommandBuffer() const noexcept { return commandBuffer; }
        [[nodiscard]] uint32 GetQueueFamily() const noexcept { return queue->GetFamily(); }

        [[nodiscard]] QueueOperations GetOperations() const noexcept { return operations; }
        [[nodiscard]] const VulkanSemaphoreValue& GetCompletionSemaphoreValue() const noexcept { return completionSemaphoreValue; } // Signalled once its last submission completes (none until first submitted)
        [[nodiscard]] std::span<const VulkanSemaphoreValue> GetAcquisitionSemaphoreValues() const noexcept { return acquisitionSemaphoreValues; } // Of swapchain images used within it

        /* --- DESTRUCTOR --- */
        ~VulkanCommandBuffer() noexcept override;

    private:
        std::string name = { };
        const VulkanQueue* queue = nullptr;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        QueueOperations operations = QueueOperations::None;
        friend class VulkanQueue;
        mutable VulkanSemaphoreValue completionSemaphoreValue = { }; // Only ever set by the queue it is submitted to
        std::vector<VulkanSemaphoreValue> acquisitionSemaphoreValues = { };

        bool debugRegionBegan = false;

        VkQueryPool timestampQueryPool = VK_NULL_HANDLE; // Only created if the queue supports timestamps
        float64 timestampPeriod = 0.0; // Nanoseconds per tick
        uint64 timestampMask = 0; // Bits of a timestamp which are valid

        bool timestampRegionBegan = false;
        bool timestampRegionStarted = false; // Whether the start of the current region has been written, which happens right before the first pass within it
        std::vector<std::string> timestampRegionNames = { }; // Of every region begun within the current recording, the [i]-th one using queries [2i] and [2i + 1]
        std::vector<TimestampRegion> timestampRegions = { };
        const VulkanResourceTable* currentResourceTable = nullptr;

        uint32 currentSubpass = 0;
        const VulkanRenderPass* currentRenderPass = nullptr;

        const VulkanGraphicsPipeline* currentGraphicsPipeline = nullptr;
        const VulkanComputePipeline* currentComputePipeline = nullptr;

        const VulkanBuffer* currentVertexBuffer = nullptr;
        const VulkanBuffer* currentIndexBuffer = nullptr;

        void WriteTimestampRegionStart();
        void ReadTimestampRegions();
        void WaitForImageAcquisition(const VulkanImage& image);

    };

}
