//
// Created by Nikolay Kanchevski on 26.01.24.
//

#include "ComputePipeline.h"

#include "Device.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create compute pipeline, as specified name must not be empty"));
        SR_THROW_IF(createInfo.pushConstantSize % 4 != 0, InvalidValueError(SR_FORMAT("Cannot create compute pipeline [{0}], as specified push constant size of [{1}] must be aligned to 4 bytes", createInfo.name, createInfo.pushConstantSize)));
        SR_THROW_IF(createInfo.pushConstantSize > Device::MAX_PUSH_CONSTANT_SIZE, InvalidValueError(SR_FORMAT("Cannot create compute pipeline [{0}], as specified push constant size of [{1}] exceeds the limit of [{2}] bytes, which is defined in [Device::MAX_PUSH_CONSTANT_SIZE]", createInfo.name, createInfo.pushConstantSize, Device::MAX_PUSH_CONSTANT_SIZE)));
    }

}