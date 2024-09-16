//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"

namespace Sierra
{

    class SIERRA_API VulkanCommandBuffer final : public CommandBuffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanCommandBuffer(const VulkanDevice& device, const CommandBufferCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        void SynchronizeBufferUsage(const Buffer& buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, size memorySize = 0, size offset = 0) override;
        void SynchronizeImageUsage(const Image& image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseLevel = 0, uint32 levelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, size memorySize = 0,  size sourceOffset = 0,  size offset = 0) override;
        void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, uint32 level = 0, uint32 layer = 0, const Vector3UInt& pixelRange = { 0, 0, 0 },  size sourceOffset = 0, const Vector3UInt& destinationPixelOffset = { 0, 0, 0 }) override;
        void GenerateMipMapsForImage(const Image& image) override;

        void BindResourceTable(const ResourceTable& resourceTable) override;
        void PushConstants(const void* memory, uint16 memorySize, uint16 offset = 0) override;

        void BeginRenderPass(const RenderPass& renderPass, std::span<const RenderPassBeginAttachment> attachments) override;
        void BeginNextSubpass(const RenderPass& renderPass) override;
        void EndRenderPass(const RenderPass& renderPass) override;

        void BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) override;
        void EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) override;

        void BindVertexBuffer(const Buffer& vertexBuffer, size offset = 0) override;
        void BindIndexBuffer(const Buffer& indexBuffer, size offset = 0) override;

        void SetScissor(const Vector4UInt& scissor) override;
        void Draw(uint32 vertexCount, uint32 vertexOffset = 0) override;
        void DrawIndexed(uint32 indexCount, uint32 indexOffset = 0, uint32 vertexOffset = 0) override;

        void BeginComputePipeline(const ComputePipeline& computePipeline) override;
        void EndComputePipeline(const ComputePipeline& computePipeline) override;

        void Dispatch(uint32 xWorkGroupCount, uint32 yWorkGroupCount = 0, uint32 zWorkGroupCount = 0) override;

        void BeginDebugRegion(std::string_view regionName, Color color = { 1.0f, 1.0f, 0.0f, 1.0f }) override;
        void InsertDebugMarker(std::string_view markerName, Color color = { 1.0f, 1.0f, 0.0f, 1.0f }) override;
        void EndDebugRegion() override;

        std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) override;
        std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; }

        [[nodiscard]] VkCommandBuffer GetVulkanCommandBuffer() const { return commandBuffer; }
        [[nodiscard]] uint64 GetCompletionSignalValue() const { return completionSignalValue; }

        /* --- DESTRUCTOR --- */
        ~VulkanCommandBuffer() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkAccessFlags BufferCommandUsageToVkAccessFlags(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static VkPipelineStageFlags BufferCommandUsageToVkPipelineStageFlags(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static VkImageLayout ImageCommandUsageToVkLayout(ImageCommandUsage imageCommandUsage);
        [[nodiscard]] static VkAccessFlags ImageCommandUsageToVkAccessFlags(ImageCommandUsage imageCommandUsage);
        [[nodiscard]] static VkPipelineStageFlags ImageCommandUsageToVkPipelineStageFlags(ImageCommandUsage imageCommandUsage);

    private:
        const VulkanDevice& device;

        std::string name;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        uint64 completionSignalValue = 0;

        VkDescriptorSet currentResourceTableDescriptorSet = VK_NULL_HANDLE;
        VkPipelineBindPoint currentBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
        VkPipelineLayout currentPipelineLayout = VK_NULL_HANDLE;

        std::queue<std::unique_ptr<Buffer>> queuedBuffersForDestruction;
        std::queue<std::unique_ptr<Image>> queuedImagesForDestruction;

    };

}
