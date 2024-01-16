//
// Created by Nikolay Kanchevski on 8.01.24.
//

#pragma once

#include "RenderingResource.h"

#include "CommandBuffer.h"
#include "Buffer.h"
#include "Image.h"

namespace Sierra
{

    class SIERRA_API Pipeline : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;
        virtual void End(std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;

        template<class T>
        inline void PushConstants(std::unique_ptr<CommandBuffer> &commandBuffer, const T &data, uint16 offset = 0) { PushConstants(commandBuffer, &data, sizeof(T), offset); }
        virtual void PushConstants(std::unique_ptr<CommandBuffer> &commandBuffer, const void* data, uint16 memoryRange, uint16 offset = 0) const = 0;
        virtual void BindBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 binding, const std::unique_ptr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0) const = 0;
        virtual void BindImage(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 binding, const std::unique_ptr<Image> &image, uint32 arrayIndex = 0) const = 0;

        /* --- OPERATORS --- */
        Pipeline(const Pipeline&) = delete;
        Pipeline &operator=(const Pipeline&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Pipeline() = default;

    protected:
        Pipeline() = default;

    };

}