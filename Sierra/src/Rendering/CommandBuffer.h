//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"
#include "ResourceTable.h"

#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"

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

        virtual void SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 byteOffset = 0) = 0;
        virtual void SynchronizeImageUsage(const std::unique_ptr<Image> &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseMipLevel = 0, uint32 mipLevelCount = 0, uint32 layer = 0, uint32 layerCount = 0) = 0;

        virtual void CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, uint64 memoryRange = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) = 0;
        virtual void CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, uint32 mipLevel = 0, const Vector2UInt &pixelRange = { 0, 0 }, uint32 layer = 0, uint64 sourceByteOffset = 0, const Vector2UInt &destinationPixelOffset = { 0, 0 }) = 0;
        virtual void GenerateMipMapsForImage(const std::unique_ptr<Image> &image) = 0;

        virtual void BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments) = 0;
        virtual void BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass) = 0;
        virtual void EndRenderPass(const std::unique_ptr<RenderPass> &renderPass) = 0;

        virtual void BindResourceTable(const std::unique_ptr<ResourceTable> &resourceTable) = 0;
        virtual void PushConstants(const void* data, uint16 memoryRange, uint16 byteOffset = 0) = 0;

        virtual void BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline) = 0;
        virtual void EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline) = 0;

        virtual void BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, uint64 byteOffset = 0) = 0;
        virtual void BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, uint64 byteOffset = 0) = 0;

        virtual void SetScissor(const Vector4UInt &scissor) = 0;
        virtual void Draw(uint32 vertexCount, uint32 vertexOffset = 0) = 0;
        virtual void DrawIndexed(uint32 indexCount, uint32 indexOffset = 0, uint32 vertexOffset = 0) = 0;

        virtual void BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline) = 0;
        virtual void EndComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline) = 0;

        virtual void Dispatch(uint32 xWorkGroupCount, uint32 yWorkGroupCount, uint32 zWorkGroupCount) = 0;

        virtual void BeginDebugRegion(std::string_view regionName, const ColorRGBA32 &color = { 1.0f, 1.0f, 0.0f, 1.0f }) = 0;
        virtual void InsertDebugMarker(std::string_view markerName, const ColorRGBA32 &color = { 1.0f, 1.0f, 0.0f, 1.0f }) = 0;
        virtual void EndDebugRegion() = 0;

        inline std::unique_ptr<Buffer>& QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) { return queuedBuffers.emplace(std::move(buffer)); }
        inline std::unique_ptr<Image>& QueueImageForDestruction(std::unique_ptr<Image> &&image) { return queuedImages.emplace(std::move(image)); }

        /* --- OPERATORS --- */
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CommandBuffer() = default;

    protected:
        explicit CommandBuffer(const CommandBufferCreateInfo &createInfo);

    protected:
        std::queue<std::unique_ptr<Buffer>> queuedBuffers;
        std::queue<std::unique_ptr<Image>> queuedImages;

    };

}
