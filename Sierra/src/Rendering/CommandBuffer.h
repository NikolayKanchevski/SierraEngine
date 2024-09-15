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

        virtual void SynchronizeBufferUsage(const Buffer& buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, size memorySize = 0, uint64 offset = 0) = 0;
        virtual void SynchronizeImageUsage(const Image& image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseLevel = 0, uint32 levelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) = 0;

        virtual void CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, uint64 memorySize = 0, uint64 sourceOffset = 0, uint64 Offset = 0) = 0;
        virtual void CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, uint32 level = 0, uint32 layer = 0, const Vector3UInt& pixelRange = { 0, 0, 0 }, uint64 sourceOffset = 0, const Vector3UInt& destinationPixelOffset = { 0, 0, 0 }) = 0;
        virtual void GenerateMipMapsForImage(const Image& image) = 0;

        virtual void BeginRenderPass(const RenderPass& renderPass, std::span<const RenderPassBeginAttachment> attachments) = 0;
        virtual void BeginNextSubpass(const RenderPass& renderPass) = 0;
        virtual void EndRenderPass(const RenderPass& renderPass) = 0;

        virtual void BindResourceTable(const ResourceTable& resourceTable) = 0;
        virtual void PushConstants(const void* memory, uint16 memorySize, uint16 offset = 0) = 0;

        virtual void BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) = 0;
        virtual void EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline) = 0;

        virtual void BindVertexBuffer(const Buffer& vertexBuffer, uint64 offset = 0) = 0;
        virtual void BindIndexBuffer(const Buffer& indexBuffer, uint64 offset = 0) = 0;

        virtual void SetScissor(const Vector4UInt& scissor) = 0;
        virtual void Draw(uint32 vertexCount, uint32 vertexOffset = 0) = 0;
        virtual void DrawIndexed(uint32 indexCount, uint32 indexOffset = 0, uint32 vertexOffset = 0) = 0;

        virtual void BeginComputePipeline(const ComputePipeline& computePipeline) = 0;
        virtual void EndComputePipeline(const ComputePipeline& computePipeline) = 0;

        virtual void Dispatch(uint32 xWorkGroupCount, uint32 yWorkGroupCount = 0, uint32 zWorkGroupCount = 0) = 0;

        virtual void BeginDebugRegion(std::string_view regionName, Color color = { 1.0f, 1.0f, 0.0f, 1.0f }) = 0;
        virtual void InsertDebugMarker(std::string_view markerName, Color color = { 1.0f, 1.0f, 0.0f, 1.0f }) = 0;
        virtual void EndDebugRegion() = 0;

        virtual std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) = 0;
        virtual std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) = 0;

        /* --- DESTRUCTOR --- */
        ~CommandBuffer() override = default;

    protected:
        explicit CommandBuffer(const CommandBufferCreateInfo& createInfo);

    };

}
