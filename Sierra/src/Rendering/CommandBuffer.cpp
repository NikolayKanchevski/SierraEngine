//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "CommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CommandBuffer::CommandBuffer(const CommandBufferCreateInfo &createInfo)
    {

    }

    /* --- PROTECTED METHODS --- */

    void CommandBuffer::FreeQueuedResources()
    {
        queriedBuffers = std::queue<std::unique_ptr<Buffer>>();
        queriedImages = std::queue<std::unique_ptr<Image>>();
    }

}