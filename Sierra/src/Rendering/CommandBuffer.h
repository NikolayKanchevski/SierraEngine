//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

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
