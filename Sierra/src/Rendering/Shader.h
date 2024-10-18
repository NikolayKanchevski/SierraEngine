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
        std::span<const uint8> memory = { };
        ShaderType shaderType = ShaderType::Undefined;
    };

    class SIERRA_API Shader : public virtual RenderingResource
    {
    public:
        /* --- COPY SEMANTICS --- */
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        /* --- MOVE SEMANTICS --- */
        Shader(Shader&&) = delete;
        Shader& operator=(Shader&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Shader() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Shader(const ShaderCreateInfo& createInfo);

        struct ShaderFileHeader
        {
            size spvMemorySize = 0;
            size macOSMetalLibMemorySize = 0;
            size iOSMetalLibMemorySize = 0;
            size iOSSimulatorMetalLibMemorySize = 0;
            size dxilMemorySize = 0;

            [[nodiscard]] constexpr size GetSpvOffset() const noexcept { return 0; }
            [[nodiscard]] size GetMacOSMetalLibOffset() const noexcept { return GetSpvOffset() + spvMemorySize; }
            [[nodiscard]] size GetIOSMetalLibOffset() const noexcept { return GetMacOSMetalLibOffset() + macOSMetalLibMemorySize; }
            [[nodiscard]] size GetIOSSimulatorMetalLibOffset() const noexcept { return GetIOSMetalLibOffset() + iOSMetalLibMemorySize; }
            [[nodiscard]] size GetDxilOffset() const noexcept { return GetIOSSimulatorMetalLibOffset() + iOSSimulatorMetalLibMemorySize; }
        };

    };

}
