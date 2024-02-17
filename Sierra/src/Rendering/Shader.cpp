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
        SR_ERROR_IF(!File::DirectoryExists(createInfo.shaderBundlePath), "Could not load shaders from bundle [{0}], as it does not exist!", createInfo.shaderBundlePath.string().c_str());
    }

}
