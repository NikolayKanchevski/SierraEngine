//
// Created by Nikolay Kanchevski on 12.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLCommandBuffer = void;
        using MTLRenderStages = ulong;
    }
#endif

#include "../CommandBuffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "MetalQueue.h"
#include "MetalBuffer.h"
#include "MetalGraphicsPipeline.h"
#include "MetalComputePipeline.h"
#include "MetalRenderPass.h"
#include "MetalFramebuffer.h"
#include "MetalResourceTable.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API MTLRenderStages BufferCommandUsageToRenderStages(BufferCommandUsage bufferCommandUsage) noexcept;
    [[nodiscard]] SIERRA_API MTLRenderStages ImageCommandUsageToRenderStages(ImageCommandUsage imageCommandUsage) noexcept;

    class SIERRA_API MetalCommandBuffer final : public CommandBuffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalCommandBuffer(const MetalQueue& queue, const CommandBufferCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        void SynchronizeBufferUsage(const Buffer& buffer, const BufferSynchronizeInfo& synchronizeInfo) override;
        void SynchronizeImageUsage(const Image& image, const ImageSynchronizeInfo& synchronizeInfo) override;

        void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const BufferToBufferCopyInfo& copyInfo) override;
        void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const BufferToImageCopyInfo& copyInfo) override;
        void GenerateMipMapsForImage(const Image& image) override;

        void BindResourceTable(const ResourceTable& resourceTable) override;
        void PushConstants(const void* memory, uint8 sourceOffset, uint8 memorySize) override;

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

        std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) override;
        std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] id<MTLCommandBuffer> GetMetalCommandBuffer() const noexcept { return commandBuffer; }
        [[nodiscard]] const MetalQueue& GetQueue() const noexcept { return *queue; }
        [[nodiscard]] uint64 GetCompletionSemaphoreSignalValue() const noexcept { return completionSemaphoreSignalValue; }

        /* --- COPY SEMANTICS --- */
        MetalCommandBuffer(const MetalCommandBuffer&) = delete;
        MetalCommandBuffer& operator=(const MetalCommandBuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalCommandBuffer(MetalCommandBuffer&&) noexcept = default;
        MetalCommandBuffer& operator=(MetalCommandBuffer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalCommandBuffer() noexcept override = default;

    private:
        const MetalQueue* queue = nullptr;
        std::string name = { };

        id<MTLCommandBuffer> commandBuffer = nil;
        uint64 completionSemaphoreSignalValue = 0;

        #if !defined(__OBJC__)
            using MTLRenderCommandEncoder = void;
            using MTLComputeCommandEncoder = void;
            using MTLBlitCommandEncoder = void;
        #endif

        id<MTLRenderCommandEncoder> currentRenderEncoder = nil;
        id<MTLComputeCommandEncoder> currentComputeEncoder = nil;
        id<MTLBlitCommandEncoder> currentBlitEncoder = nil;

        bool debugRegionBegan = false;
        const MetalResourceTable* currentResourceTable = nullptr;

        int32 currentSubpass = -1;
        const MetalRenderPass* currentRenderPass = nullptr;
        const MetalFramebuffer* currentFramebuffer = nullptr;

        const MetalGraphicsPipeline* currentGraphicsPipeline = nullptr;
        const MetalComputePipeline* currentComputePipeline = nullptr;

        uint64 initialVertexBufferOffset = 0;
        const MetalBuffer* currentVertexBuffer = nullptr;

        uint64 initialIndexBufferOffset = 0;
        const MetalBuffer* currentIndexBuffer = nullptr;

        std::queue<std::unique_ptr<Buffer>> queuedBuffersForDestruction;
        std::queue<std::unique_ptr<Image>> queuedImagesForDestruction;

        void UpdateBlitEncoder();

    };

}
