//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "Buffer.h"
#include "ComputePipeline.h"
#include "GraphicsPipeline.h"
#include "Image.h"
#include "RenderingResource.h"
#include "RenderPass.h"
#include "ResourceTable.h"

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

    class SIERRA_API CommandBuffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void SynchronizeBufferUsage(const Buffer& buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, size offset, size memorySize);
        virtual void SynchronizeImageUsage(const Image& image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseLevel, uint32 levelCount, uint32 baseLayer, uint32 layerCount);

        virtual void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, size sourceOffset, size destinationOffset, size memorySize);
        virtual void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, uint32 level, uint32 layer, size sourceOffset, Vector3UInt destinationPixelOffset, Vector3UInt pixelRange);
        virtual void GenerateMipMapsForImage(const Image& image);

        virtual void BindResourceTable(const ResourceTable& resourceTable);
        virtual void PushConstants(const void* memory, size offset, size memorySize);

        virtual void BeginRenderPass(const RenderPass& renderPass, std::span<const RenderPassBeginAttachment> attachments);
        virtual void BeginNextSubpass(const RenderPass& renderPass);
        virtual void EndRenderPass(const RenderPass& renderPass);

        virtual void BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline);
        virtual void EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline);

        virtual void BindVertexBuffer(const Buffer& vertexBuffer, size offset);
        virtual void BindIndexBuffer(const Buffer& indexBuffer, size offset);

        virtual void SetScissor(Vector4UInt scissor);
        virtual void Draw(uint32 vertexCount, size vertexOffset);
        virtual void DrawIndexed(uint32 indexCount, size indexOffset, size vertexOffset);

        virtual void BeginComputePipeline(const ComputePipeline& computePipeline);
        virtual void EndComputePipeline(const ComputePipeline& computePipeline);

        virtual void Dispatch(Vector3UInt workGroupSize);

        virtual void BeginDebugRegion(std::string_view regionName, Color32 color);
        virtual void InsertDebugMarker(std::string_view markerName, Color32 color);
        virtual void EndDebugRegion();

        virtual std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) = 0;
        virtual std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) = 0;

        /* --- COPY SEMANTICS --- */
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        CommandBuffer(CommandBuffer&&) = delete;
        CommandBuffer& operator=(CommandBuffer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~CommandBuffer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit CommandBuffer(const CommandBufferCreateInfo& createInfo);

    private:

    };

}
