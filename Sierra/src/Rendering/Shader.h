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
        /* --- DESTRUCTOR --- */
        ~Shader() override = default;

    protected:
        explicit Shader(const ShaderCreateInfo& createInfo);
        struct ShaderFileHeader
        {
            uint64 spvMemorySize = 0;
            uint64 macOSMetalLibMemorySize = 0;
            uint64 iOSMetalLibMemorySize = 0;
            uint64 iOSSimulatorMetalLibMemorySize = 0;
            uint64 dxilMemorySize = 0;

            [[nodiscard]] constexpr uint64 GetSpvOffset() const { return 0; }
            [[nodiscard]] uint64 GetMacOSMetalLibOffset() const { return GetSpvOffset() + spvMemorySize; }
            [[nodiscard]] uint64 GetIOSMetalLibOffset() const { return GetMacOSMetalLibOffset() + macOSMetalLibMemorySize; }
            [[nodiscard]] uint64 GetIOSSimulatorMetalLibOffset() const { return GetIOSMetalLibOffset() + iOSMetalLibMemorySize; }
            [[nodiscard]] uint64 GetDxilOffset() const { return GetIOSSimulatorMetalLibOffset() + iOSSimulatorMetalLibMemorySize; }
        };

    };

}
