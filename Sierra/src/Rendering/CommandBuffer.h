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

        /* --- OPERATORS --- */
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

    protected:
        explicit CommandBuffer(const CommandBufferCreateInfo &createInfo);

    };

}
