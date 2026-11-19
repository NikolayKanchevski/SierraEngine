//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "Image.h"
#include "Buffer.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "ResourceTable.h"
#include "ComputePipeline.h"
#include "GraphicsPipeline.h"
#include "RenderingResource.h"
#include "../Utilities/TimeStep.h"

namespace Sierra
{

    enum class BufferCommandUsage : uint8
    {
        None,
        MemoryRead,
        MemoryWrite,
        VertexRead,
        IndexRead,
        GraphicsRead,
        GraphicsWrite,
        ComputeRead,
        ComputeWrite
    };

    enum class ImageCommandUsage : uint8
    {
        None,
        MemoryRead,
        MemoryWrite,
        ColorRead,
        ColorWrite,
        DepthRead,
        DepthWrite,
        GraphicsRead,
        GraphicsWrite,
        ComputeRead,
        ComputeWrite,
        Present
    };

    struct CommandBufferCreateInfo
    {
        std::string_view name = "Command Buffer";
    };

    struct BufferSynchronizeInfo
    {
        BufferCommandUsage previousUsage = BufferCommandUsage::None;
        BufferCommandUsage nextUsage = BufferCommandUsage::None;

        uint64 offset = 0;
        uint64 memorySize = 0;
    };

    struct ImageSynchronizeInfo
    {
        ImageCommandUsage previousUsage = ImageCommandUsage::None;
        ImageCommandUsage nextUsage = ImageCommandUsage::None;

        uint32 baseLevel = 0;
        uint32 levelCount = 1;

        uint32 baseLayer = 0;
        uint32 layerCount = 1;
    };

    struct BufferToBufferCopyInfo
    {
        uint64 sourceOffset = 0;
        uint64 destinationOffset = 0;
        uint64 memorySize = 0;
    };

    struct BufferToImageCopyInfo
    {
        uint32 level = 0;
        uint32 layer = 0;

        uint64 sourceOffset = 0;
        Vector3UInt destinationPixelOffset = { 0, 0, 0 };
        Vector3UInt pixelRange = { 0, 1, 1 };
    };

    struct TimestampRegion
    {
        std::string name = { };
        TimeStep duration = { }; // GPU time from the start of the first pass within the region, to the end of the last one
    };

    class SIERRA_API CommandBuffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void SynchronizeBufferUsage(const Buffer& buffer, const BufferSynchronizeInfo& synchronizeInfo);
        virtual void SynchronizeImageUsage(const Image& image, const ImageSynchronizeInfo& synchronizeInfo);

        virtual void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const BufferToBufferCopyInfo& copyInfo);
        virtual void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const BufferToImageCopyInfo& copyInfo);
        virtual void GenerateMipMapsForImage(const Image& image);

        virtual void BindResourceTable(const ResourceTable& resourceTable);
        virtual void PushConstants(std::span<const uint8> buffer);

        template<typename T>
        void PushConstantsAs(const T& t)
        {
            PushConstants({ reinterpret_cast<const uint8*>(&t), sizeof(T) });
        }

        virtual void BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer);
        virtual void BeginNextSubpass();
        virtual void EndRenderPass();

        virtual void BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline);
        virtual void EndGraphicsPipeline();

        virtual void BindVertexBuffer(const Buffer& vertexBuffer, uint64 offset);
        virtual void BindIndexBuffer(const Buffer& indexBuffer, uint64 offset);

        virtual void SetScissor(Vector4UInt scissor);
        virtual void Draw(uint32 vertexCount, uint64 vertexOffset);
        virtual void DrawIndexed(uint32 indexCount, uint64 indexOffset, uint64 vertexOffset);

        virtual void BeginComputePipeline(const ComputePipeline& computePipeline);
        virtual void EndComputePipeline();

        virtual void Dispatch(Vector3UInt workGroupSize);

        virtual void BeginDebugRegion(std::string_view regionName, Color32 color);
        virtual void InsertDebugMarker(std::string_view markerName, Color32 color);
        virtual void EndDebugRegion();

        // NOTE: A timestamp region measures the passes (render passes, compute pipelines, copies and mip map generations) recorded within it. As Metal can only sample timestamps
        // between encoders, every backend allows regions to only be begun and ended outside of render passes and compute pipelines, not to be nested, and requires them ended before
        // the command buffer is, while regions with no pass within them are not measured at all
        virtual void BeginTimestampRegion(std::string_view regionName);
        virtual void EndTimestampRegion();

        /* --- GETTER METHODS --- */
        // Measured regions of the last recording to have finished executing, in the order they were recorded in (read back once the command buffer is begun again)
        [[nodiscard]] virtual std::span<const TimestampRegion> GetTimestampRegions() const noexcept { return { }; }

        /* --- CONSTANTS --- */
        constexpr static uint32 TIMESTAMP_QUERY_POOL_SIZE = 32;
        constexpr static uint32 MAX_TIMESTAMP_REGION_COUNT = TIMESTAMP_QUERY_POOL_SIZE / 2; // Every region takes two timestamps, one at its start and one at its end

        /* --- COPY SEMANTICS --- */
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /* --- DESTRUCTOR --- */
        ~CommandBuffer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit CommandBuffer(const CommandBufferCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        CommandBuffer(CommandBuffer&&) noexcept = default;
        CommandBuffer& operator=(CommandBuffer&&) noexcept = default;

    };

}
