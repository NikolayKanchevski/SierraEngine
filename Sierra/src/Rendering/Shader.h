//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class ShaderType : uint8
    {
        Undefined,
        Vertex,
        Fragment,
        Compute
    };

    struct ShaderCreateInfo
    {
        std::string_view name = "Shader";
        ShaderType shaderType = ShaderType::Undefined;
        std::span<const uint8> code = { };
    };

    class SIERRA_API Shader : public virtual RenderingResource
    {
    public:
        /* --- COPY SEMANTICS --- */
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        /* --- DESTRUCTOR --- */
        ~Shader() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Shader(const ShaderCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        Shader(Shader&&) noexcept = default;
        Shader& operator=(Shader&&) noexcept = default;

    };

}
