//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "GraphicsPipeline.h"

#include "Device.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.pushConstantSize % 4 != 0, "Cannot create graphics pipeline [{0}], as specified push constant size of [{1}] bytes must be aligned to 4 bytes!", createInfo.name, createInfo.pushConstantSize);
        SR_ERROR_IF(createInfo.pushConstantSize > Device::MAX_PUSH_CONSTANT_SIZE, "Cannot create graphics pipeline [{0}], as specified push constant size of [{1}] bytes exceeds the limit of [{2}] bytes, which is defined in [Device::MAX_PUSH_CONSTANT_SIZE]!", createInfo.name, createInfo.pushConstantSize, Device::MAX_PUSH_CONSTANT_SIZE);
    }

}