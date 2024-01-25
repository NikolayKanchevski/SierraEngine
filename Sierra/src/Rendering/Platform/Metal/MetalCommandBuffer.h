//
// Created by Nikolay Kanchevski on 12.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "MetalGraphicsPipeline.h"

namespace Sierra
{

    class SIERRA_API MetalCommandBuffer final : public CommandBuffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalCommandBuffer(const MetalDevice &device, const CommandBufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        void SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 offset = 0) override;
        void SynchronizeImageUsage(const std::unique_ptr<Image> &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseMipLevel = 0, uint32 mipLevelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, uint64 memoryRange = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) override;
        void CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, const Vector2UInt &pixelRange = { 0, 0 }, uint32 sourceOffset = 0, const Vector2UInt &destinationOffset = { 0, 0 }, uint32 mipLevel = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments) override;
        void BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass) override;
        void EndRenderPass(const std::unique_ptr<RenderPass> &renderPass) override;

        void BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &pipeline) override;
        void EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &pipeline) override;

        void BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, uint64 offset = 0) override;
        void BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, uint64 offset = 0) override;

        void Draw(uint32 vertexCount) override;
        void DrawIndexed(uint32 indexCount, uint64 indexOffset = 0, uint64 vertexOffset = 0) override;

        void PushConstants(const void* data, uint16 memoryRange, uint16 offset = 0) override;
        void BindBuffer(uint32 binding, const std::unique_ptr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0) override;
        void BindImage(uint32 binding, const std::unique_ptr<Image> &image, uint32 arrayIndex = 0) override;

        void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) override;
        void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) override;
        void EndDebugRegion() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MTL::CommandBuffer* GetMetalCommandBuffer() const { return commandBuffer; }
        [[nodiscard]] inline bool HasFinishedExecution() const { return finishedExecution; }
        [[nodiscard]] inline MTL::RenderCommandEncoder* GetCurrentRenderEncoder() const { return currentRenderEncoder; }

        [[nodiscard]] inline MTL::Buffer* GetCurrentIndexBuffer() const { return currentIndexBuffer; }
        [[nodiscard]] inline uint64 GetCurrentIndexBufferOffset() const { return currentIndexBufferOffset; }

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTL::RenderStages BufferCommandUsageToRenderStages(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static MTL::RenderStages ImageCommandUsageToRenderStages(ImageCommandUsage imageCommandUsage);

        /* --- DESTRUCTOR --- */
        ~MetalCommandBuffer() override = default;

    private:
        const MetalDevice &device;
        MTL::CommandBuffer* commandBuffer = nullptr;
        std::atomic<bool> finishedExecution = true;

        MTL::RenderCommandEncoder* currentRenderEncoder = nullptr;
        uint32 currentSubpass = 0;

        const MetalGraphicsPipeline* currentGraphicsPipeline = nullptr;

        MTL::Buffer* currentIndexBuffer = nullptr;
        uint64 currentIndexBufferOffset = 0;

    };

}
