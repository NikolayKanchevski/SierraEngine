//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "../Core/FileManager.h"

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
        std::span<const uint8> data = { };
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
        struct ShaderFileHeader
        {
            uint64 spvMemorySize = 0;
            uint64 macOSMetalLibMemorySize = 0;
            uint64 iOSMetalLibMemorySize = 0;
            uint64 iOSSimulatorMetalLibMemorySize = 0;
            uint64 dxilMemorySize = 0;

            [[nodiscard]] constexpr inline uint64 GetSpvOffset() const { return 0; }
            [[nodiscard]] inline uint64 GetMacOSMetalLibOffset() const { return GetSpvOffset() + spvMemorySize; }
            [[nodiscard]] inline uint64 GetIOSMetalLibOffset() const { return GetMacOSMetalLibOffset() + macOSMetalLibMemorySize; }
            [[nodiscard]] inline uint64 GetIOSSimulatorMetalLibOffset() const { return GetIOSMetalLibOffset() + iOSMetalLibMemorySize; }
            [[nodiscard]] inline uint64 GetDxilOffset() const { return GetIOSSimulatorMetalLibOffset() + iOSSimulatorMetalLibMemorySize; }
        };

    };

}
