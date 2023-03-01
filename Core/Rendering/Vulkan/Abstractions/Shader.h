//
// Created by Nikolay Kanchevski on 18.12.22.
//

#pragma once

#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    template<typename A, typename B, typename C>
    class GraphicsPipeline;

    struct CompiledShaderCreateInfo
    {
        String filePath;
        ShaderType shaderType;

        const char* entryPoint = "main";
    };

    enum ShaderOptimization
    {
        None = shaderc_optimization_level_zero,
        OptimizeSize = shaderc_optimization_level_size,
        OptimizePerformance = shaderc_optimization_level_performance
    };

    struct ShaderDefinition
    {
        String definitionName;
        String value;
    };

    struct ShaderCreateInfo
    {
        String filePath;
        ShaderType shaderType = ShaderType::NONE;

        std::vector<ShaderDefinition> definitions;
        ShaderOptimization optimization = ShaderOptimization::OptimizeSize;

        const char* entryPoint = "main";
    };

    class Shader
    {
    public:
        /* --- CONSTRUCTORS --- */
        Shader(const CompiledShaderCreateInfo &createInfo);
        static SharedPtr<Shader> LoadCompiled(CompiledShaderCreateInfo createInfo);

        Shader(const ShaderCreateInfo &createInfo);
        static SharedPtr<Shader> Create(ShaderCreateInfo createInfo);

        /* --- SETTER METHODS --- */
        void Dispose();
        static void DisposePool();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline String GetFilePath() const { return filePath; }
        [[nodiscard]] inline VkShaderModule GetVkShaderModule() const { return shaderModule; }
        [[nodiscard]] inline VkPipelineShaderStageCreateInfo& GetVkShaderStageInfo() { return shaderStageCreateInfo; }
        [[nodiscard]] inline ShaderType GetShaderType() const { return shaderType; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        template<typename A, typename B, typename C>
        friend class GraphicsPipeline;
        DELETE_COPY(Shader);

    private:
        class Includer : public shaderc::CompileOptions::IncluderInterface
        {
        public:
            Includer(const String givenFilePath);

            shaderc_include_result* GetInclude(const char* requestedSource, shaderc_include_type type, const char* requestingSource, size_t includeDepth) override;
            void ReleaseInclude(shaderc_include_result* data) override;
        private:
            String filePath;

        };

        String filePath = "";
        ShaderType shaderType;

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};

        std::vector<uint> CompileShadercShader();
        void CreateShaderModule(const char* entryPoint, const uint* code, uint codeSize);

        ShaderType GetShaderTypeFromExtension();
        shaderc_shader_kind GetShadercShaderType();

        struct PrecompileData
        {
            // Name | Value
            std::unordered_map<String, String> definitions;
            ShaderOptimization optimization;
            const char* entryPoint;
        };

        PrecompileData* precompiledData;
        bool precompiled;

        inline static shaderc::Compiler compiler;

        // filePath | Shader object
        inline static std::unordered_map<String, SharedPtr<Shader>> shaderPool;

        bool SetDefinition(const ShaderDefinition &definition);
    };

}