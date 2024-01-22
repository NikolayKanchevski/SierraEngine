//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"

namespace Sierra
{

    enum class BufferCommandUsage
    {
        MemoryRead,
        MemoryWrite,
        VertexRead,
        IndexRead,
        GraphicsRead,
        GraphicsWrite,
        ComputeRead,
        ComputeWrite
    };

    enum class ImageCommandUsage
    {
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

        virtual void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const = 0;
        virtual void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const = 0;
        virtual void EndDebugRegion() const = 0;

        /* --- OPERATORS --- */
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CommandBuffer() = default;

    protected:
        explicit CommandBuffer(const CommandBufferCreateInfo &createInfo);

    };

}
