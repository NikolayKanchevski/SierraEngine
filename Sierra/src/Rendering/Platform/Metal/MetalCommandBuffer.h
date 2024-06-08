//
// Created by Nikolay Kanchevski on 12.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLCommandBuffer = void;
        using MTLRenderStages = std::uintptr_t;
    }
#endif

#include "../../CommandBuffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "MetalGraphicsPipeline.h"
#include "MetalComputePipeline.h"
#include "MetalResourceTable.h"

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

        void SynchronizeBufferUsage(const Buffer &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 byteOffset = 0) override;
        void SynchronizeImageUsage(const Image &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseLevel = 0, uint32 levelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void CopyBufferToBuffer(const Buffer &sourceBuffer, const Buffer &destinationBuffer, uint64 memoryRange = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) override;
        void CopyBufferToImage(const Buffer &sourceBuffer, const Image &destinationImage, uint32 level = 0, uint32 layer = 0, const Vector3UInt &pixelRange = { 0, 0, 0 }, uint64 sourceByteOffset = 0, const Vector3UInt &destinationPixelOffset = { 0, 0, 0 }) override;
        void GenerateMipMapsForImage(const Image &image) override;

        void BindResourceTable(const ResourceTable &resourceTable) override;
        void PushConstants(const void* data, uint16 memoryRange, uint16 byteOffset = 0) override;

        void BeginRenderPass(const RenderPass &renderPass, std::span<const RenderPassBeginAttachment> attachments) override;
        void BeginNextSubpass(const RenderPass &renderPass) override;
        void EndRenderPass(const RenderPass &renderPass) override;

        void BeginGraphicsPipeline(const GraphicsPipeline &pipeline) override;
        void EndGraphicsPipeline(const GraphicsPipeline &pipeline) override;

        void BindVertexBuffer(const Buffer &vertexBuffer, uint64 byteOffset = 0) override;
        void BindIndexBuffer(const Buffer &indexBuffer, uint64 byteOffset = 0) override;

        void SetScissor(const Vector4UInt &scissor) override;
        void Draw(uint32 vertexCount, uint32 vertexOffset = 0) override;
        void DrawIndexed(uint32 indexCount, uint32 indexOffset = 0, uint32 vertexOffset = 0) override;

        void BeginComputePipeline(const ComputePipeline &computePipeline) override;
        void EndComputePipeline(const ComputePipeline &computePipeline) override;

        void Dispatch(uint32 xWorkGroupCount, uint32 yWorkGroupCount, uint32 zWorkGroupCount) override;

        void BeginDebugRegion(std::string_view regionName, const ColorRGBA32 &color = { 1.0f, 1.0f, 0.0f, 1.0f }) override;
        void InsertDebugMarker(std::string_view markerName, const ColorRGBA32 &color = { 1.0f, 1.0f, 0.0f, 1.0f }) override;
        void EndDebugRegion() override;

        std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) override;
        std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] id<MTLCommandBuffer> GetMetalCommandBuffer() const { return commandBuffer; }
        [[nodiscard]] uint64 GetCompletionSignalValue() const { return completionSignalValue; }

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTLRenderStages BufferCommandUsageToRenderStages(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static MTLRenderStages ImageCommandUsageToRenderStages(ImageCommandUsage imageCommandUsage);

        /* --- DESTRUCTOR --- */
        ~MetalCommandBuffer() override;

    private:
        const MetalDevice &device;
        #if !defined(__OBJC__)
            using MTLRenderCommandEncoder = void;
            using MTLComputeCommandEncoder = void;
            using MTLBlitCommandEncoder = void;
        #endif

        id<MTLCommandBuffer> commandBuffer = nil;
        uint64 completionSignalValue = 0;

        id<MTLRenderCommandEncoder> currentRenderEncoder = nil;
        id<MTLComputeCommandEncoder> currentComputeEncoder = nil;
        id<MTLBlitCommandEncoder> currentBlitEncoder = nil;
        uint32 currentSubpass = 0;

        const MetalGraphicsPipeline* currentGraphicsPipeline = nullptr;
        const MetalComputePipeline* currentComputePipeline = nullptr;
        const MetalResourceTable* currentResourceTable = nullptr;

        id<MTLBuffer> currentIndexBuffer = nil;
        uint64 currentIndexBufferByteOffset = 0;
        uint64 currentVertexBufferByteOffset = 0;

        std::queue<std::unique_ptr<Buffer>> queuedBuffersForDestruction;
        std::queue<std::unique_ptr<Image>> queuedImagesForDestruction;


    };

}
