//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

namespace SierraEngine
{

    struct ShaderCompileInfo
    {
        Sierra::FileStream& fileStream;
        const Sierra::FileManager& fileManager;
    };

    struct ShaderEntryPoint
    {
        std::string name = { };
        Sierra::ShaderType type = Sierra::ShaderType::Undefined;

        size spirvCodeOffset = 0;
        size spirvCodeMemorySize = 0;

        size macOSMetalCodeOffset = 0;
        size macOSMetalCodeMemorySize = 0;

        size iOSMetalCodeOffset = 0;
        size iOSMetalCodeMemorySize = 0;
    };

    struct CompiledShader
    {
        std::vector<ShaderEntryPoint> entryPoints = { };
        std::vector<uint8> buffer = { };
    };

    class SIERRA_ENGINE_API ShaderCompiler
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<CompiledShader> Compile(const ShaderCompileInfo& compileInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        ShaderCompiler(const ShaderCompiler&) = delete;
        ShaderCompiler& operator=(const ShaderCompiler&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ShaderCompiler() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ShaderCompiler() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ShaderCompiler(ShaderCompiler&&) noexcept = default;
        ShaderCompiler& operator=(ShaderCompiler&&) noexcept = default;

    };

}
