//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "Buffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Buffer::Buffer(const BufferCreateInfo &createInfo)
        : memorySize(createInfo.memorySize)
    {
        SR_ERROR_IF(createInfo.memorySize == 0, "Memory size of buffer [{0}] must not be [0] bytes!", createInfo.name);
        SR_ERROR_IF(createInfo.usage == BufferUsage::Undefined, "Usage of buffer [{0}] must not be [BufferUsage::Undefined]!", createInfo.name);
    }

}