//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "Shader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Shader::Shader(const ShaderCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create shader, as specified name must not be empty"));
        SR_THROW_IF(createInfo.shaderType == ShaderType::Undefined, InvalidValueError(SR_FORMAT("Cannot create shader [{0}], as specified type must not be [ShaderType::Undefined]", createInfo.name)));
        SR_THROW_IF(createInfo.memory.empty(), InvalidValueError(SR_FORMAT("Cannot create shader [{0}], as specified memory must not be empty", createInfo.name)));
    }

}
