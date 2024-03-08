//
// Created by Nikolay Kanchevski on 9.01.24.
//

#include "PipelineLayout.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    PipelineLayout::PipelineLayout(const PipelineLayoutCreateInfo &createInfo)
    {
        #if SR_ENABLE_LOGGING
            for (uint32 i = 0; i < createInfo.bindings.size(); i++)
            {
                const auto* iterator = createInfo.bindings.begin() + i;
                SR_ERROR_IF(iterator->type == PipelineBindingType::Undefined, "Cannot create pipeline layout [{0}], as its [{1}] binding must not be of type PipelineBindingType::Undefined!", createInfo.name, i);
                SR_ERROR_IF(iterator->arraySize == 0, "Cannot create pipeline layout [{0}], as its [{1}] binding must not have an array size of 0!", createInfo.name, i);
            }
        #endif
        SR_ERROR_IF(createInfo.pushConstantSize > 128, "Cannot create graphics pipeline [{0}] with a push constant size of [{1}], as it exceeds the maximum allowed push constant size of [128] bytes!", createInfo.name, createInfo.pushConstantSize);
        SR_ERROR_IF(createInfo.pushConstantSize % 4 != 0, "Cannot create graphics pipeline [{0}] with a push constant size of [{1}], as it must be aligned to 4 bytes!", createInfo.name, createInfo.pushConstantSize);
    }

}