//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

namespace slang
{
    struct IGlobalSession;
}

namespace SierraEngine
{

    // Compiles Slang to SPIR-V with Slang, and the SPIR-V on to Metal with SPIRV-Cross, using the bindless layout the Metal backend expects
    class SlangShaderCompiler final : public ShaderCompiler
    {
    public:
        /* --- CONSTRUCTORS --- */
        SlangShaderCompiler();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<CompiledShader> Compile(const ShaderCompileInfo& compileInfo) const override;

        /* --- CONSTANTS --- */
        constexpr static std::string_view SOURCE_FILE_EXTENSION = ".slang";

        /* --- COPY SEMANTICS --- */
        SlangShaderCompiler(const SlangShaderCompiler&) = delete;
        SlangShaderCompiler& operator=(const SlangShaderCompiler&) = delete;

        /* --- MOVE SEMANTICS --- */
        SlangShaderCompiler(SlangShaderCompiler&&) = delete;
        SlangShaderCompiler& operator=(SlangShaderCompiler&&) = delete;

        /* --- DESTRUCTOR --- */
        ~SlangShaderCompiler() noexcept override;

    private:
        // NOTE: Creating it is slow, so a single one is kept, while every compilation gets a session of its own, so that edited imports are never served from a cache
        slang::IGlobalSession* globalSession = nullptr;

    };

}
