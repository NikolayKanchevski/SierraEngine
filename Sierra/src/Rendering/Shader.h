//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class ShaderType
    {
        Undefined,
        Vertex,
        Fragment,
        Compute
    };

    struct ShaderCreateInfo
    {
        const std::string &name = "Shader";
        const std::filesystem::path &shaderBundlePath;
        ShaderType shaderType = ShaderType::Undefined;
    };

    class SIERRA_API Shader : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Shader() = default;

    protected:
        explicit Shader(const ShaderCreateInfo &createInfo);

    };

}
