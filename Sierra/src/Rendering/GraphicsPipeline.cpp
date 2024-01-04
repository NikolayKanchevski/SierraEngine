//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "GraphicsPipeline.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.pushConstantSize > 128, "Cannot create graphics pipeline [{0}] with a push constant size of [{1}], as it exceeds the maximum allowed push constant size of [128] bytes!", createInfo.name, createInfo.pushConstantSize);
    }

}