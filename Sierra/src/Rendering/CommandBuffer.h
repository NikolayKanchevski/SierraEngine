//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"

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
        AttachmentRead,
        AttachmentWrite,
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
        const std::string &name = "Command Buffer";
    };

    class SIERRA_API CommandBuffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 offset = 0) = 0;
        virtual void SynchronizeImageUsage(const std::unique_ptr<Image> &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseMipLevel = 0, uint32 mipLevelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) = 0;

        virtual void CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, uint64 memoryRange = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) = 0;
        virtual void CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, const Vector2UInt &pixelRange = { 0, 0 }, uint32 sourcePixelOffset = 0, const Vector2UInt &destinationOffset = { 0, 0 }, uint32 mipLevel = 0, uint32 baseLayer = 0, uint32 layerCount = 0) = 0;
        virtual void BlitImage(const std::unique_ptr<Image> &image) = 0;

        virtual void BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments) = 0;
        virtual void BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass) = 0;
        virtual void EndRenderPass(const std::unique_ptr<RenderPass> &renderPass) = 0;

        virtual void BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline) = 0;
        virtual void EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline) = 0;

        virtual void BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, uint64 offset = 0) = 0;
        virtual void BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, uint64 offset = 0) = 0;

        virtual void Draw(uint32 vertexCount) = 0;
        virtual void DrawIndexed(uint32 indexCount, uint64 indexOffset = 0, uint64 vertexOffset = 0) = 0;

        virtual void BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline) = 0;
        virtual void EndComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline) = 0;

        virtual void Dispatch(uint32 xWorkGroupCount, uint32 yWorkGroupCount, uint32 zWorkGroupCount) = 0;

        virtual void PushConstants(const void* data, uint16 memoryRange, uint16 offset = 0) = 0;
        virtual void BindBuffer(uint32 binding, const std::unique_ptr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0) = 0;
        virtual void BindImage(uint32 binding, const std::unique_ptr<Image> &image, uint32 arrayIndex = 0) = 0;
        virtual void BindImage(uint32 binding, const std::unique_ptr<Image> &image, const std::unique_ptr<Sampler> &sampler, uint32 arrayIndex = 0) = 0;

        virtual void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) = 0;
        virtual void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) = 0;
        virtual void EndDebugRegion() = 0;

        inline void QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer) { queriedBuffers.emplace(std::move(buffer)); }
        inline void QueueImageForDestruction(std::unique_ptr<Image> &&image) { queriedImages.emplace(std::move(image)); }

        /* --- OPERATORS --- */
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CommandBuffer() = default;

    protected:
        explicit CommandBuffer(const CommandBufferCreateInfo &createInfo);
        void FreeQueuedResources();

    private:
        std::queue<std::unique_ptr<Buffer>> queriedBuffers;
        std::queue<std::unique_ptr<Image>> queriedImages;

    };

}
