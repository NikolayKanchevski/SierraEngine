//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "Shader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Shader::Shader(const ShaderCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.shaderType == ShaderType::Undefined, "Shader type of shader [{0}] must not be ShaderType::Undefined!", createInfo.name);
        SR_ERROR_IF(createInfo.data.empty() || createInfo.data.data() == nullptr, "Shader data of shader [{0}] must not be empty and contain a valid memory pointer!", createInfo.name);
    }

}
