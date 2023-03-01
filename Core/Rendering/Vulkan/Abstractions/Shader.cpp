//
// Created by Nikolay Kanchevski on 18.12.22.
//

#include "Shader.h"

#include "../VK.h"
#include "../../../../Engine/Classes/File.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Shader::Shader(const ShaderCreateInfo &createInfo)
        : filePath(createInfo.filePath), shaderType(createInfo.shaderType != ShaderType::NONE ? createInfo.shaderType : GetShaderTypeFromExtension()), precompiled(true)
    {
        // Save data
        precompiledData = new PrecompileData();
        precompiledData->optimization = std::move(createInfo.optimization);
        precompiledData->entryPoint = std::move(createInfo.entryPoint);
        for (const auto &pair : createInfo.definitions)
        {
            precompiledData->definitions[pair.definitionName] = pair.value;
        }

        auto code = CompileShadercShader();
        CreateShaderModule(precompiledData->entryPoint, code.data(), code.size());
    }

    SharedPtr<Shader> Shader::Create(const ShaderCreateInfo createInfo)
    {
        // Check if shader has already been loaded and return it
        if (shaderPool.count(createInfo.filePath))
        {
            return shaderPool[createInfo.filePath];
        }

        // Create and store new shader in pool
        auto shader = std::make_shared<Shader>(createInfo);
        shaderPool[createInfo.filePath] = shader;

        return shader;
    }

    Shader::Shader(const CompiledShaderCreateInfo &createInfo)
        : filePath(createInfo.filePath), shaderType(createInfo.shaderType), precompiled(false)
    {
        auto code = File::ReadBinaryFile(filePath);
        std::vector<uint> uintCode = { code.begin(), code.end() };
        CreateShaderModule(createInfo.entryPoint, uintCode.data(), uintCode.size());
    }

    SharedPtr<Shader> Shader::LoadCompiled(CompiledShaderCreateInfo createInfo)
    {
        // Check if shader has already been loaded and return it
        if (shaderPool.count(createInfo.filePath))
        {
            return shaderPool[createInfo.filePath];
        }

        // Create and store new shader in pool
        auto shader = std::make_shared<Shader>(createInfo);
        shaderPool[createInfo.filePath] = shader;

        return shader;
    }

    /* --- DISPOSE POOL --- */

    void Shader::Dispose()
    {
        shaderPool.erase(filePath);
    }

    void Shader::DisposePool()
    {
        for (const auto &shader : shaderPool)
        {
            shader.second->Destroy();
        }

        shaderPool.clear();
    }

    /* --- PRIVATE METHODS --- */

    bool Shader::SetDefinition(const ShaderDefinition &definition)
    {
        if (!precompiled)
        {
            ASSERT_ERROR_FORMATTED("Cannot set a definition of shader [{0}] that has not been precompiled! The feature is only available on raw shaders (not .spv)", filePath);
            return false;
        }

        precompiledData->definitions[definition.definitionName] = definition.value;

        auto code = CompileShadercShader();
        CreateShaderModule(precompiledData->entryPoint, code.data(), code.size());

        return true;
    }

    std::vector<uint> Shader::CompileShadercShader()
    {
        // Set options
        shaderc::CompileOptions options;
        options.SetIncluder(std::make_unique<Includer>(filePath));

        // Define macros
        for (const auto &pair : precompiledData->definitions)
        {
            options.AddMacroDefinition(pair.first, pair.second);
        }

        // Set the optimization
        if (precompiledData->optimization != ShaderOptimization::None) options.SetOptimizationLevel((shaderc_optimization_level) precompiledData->optimization);

        // Read and compile shader
        auto shaderCode = File::ReadFile(filePath);
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderCode.data(), shaderCode.size(),GetShadercShaderType(), File::GetFileNameFromPath(filePath).c_str(), options);

        // Check for errors
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ASSERT_ERROR_FORMATTED("Precompiling shader [{0}] failed with an error code of [{1}]: {2}", filePath, module.GetCompilationStatus(), module.GetErrorMessage());
        }

        return { module.begin(), module.end() };
    }

    void Shader::CreateShaderModule(const char* entryPoint, const uint* code, const uint codeSize)
    {
        // Destroy module if already created
        if (shaderModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(VK::GetLogicalDevice(), shaderModule, nullptr);
        }

        // Set module creation info
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = codeSize * UINT_SIZE;
        moduleCreateInfo.pCode = code;

        // Create shader module
        VK_ASSERT(
            vkCreateShaderModule(VK::GetLogicalDevice(), &moduleCreateInfo, nullptr, &shaderModule),
            fmt::format("Failed to create shader module for [{0}]", filePath)
        );

        // Setup shader stage creation info
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.stage = (VkShaderStageFlagBits) shaderType;
        shaderStageCreateInfo.module = shaderModule;
        shaderStageCreateInfo.pName = entryPoint;
    }

    ShaderType Shader::GetShaderTypeFromExtension()
    {
        if (filePath.rfind(".vert", filePath.size() - 1) != String::npos) return ShaderType::VERTEX;
        if (filePath.rfind(".frag", filePath.size() - 1) != String::npos) return ShaderType::FRAGMENT;
        if (filePath.rfind(".geom", filePath.size() - 1) != String::npos) return ShaderType::GEOMETRY;
        if (filePath.rfind(".tese", filePath.size() - 1) != String::npos) return ShaderType::TESSELATION_EVALUATION;
        if (filePath.rfind(".tesc", filePath.size() - 1) != String::npos) return ShaderType::TESSELATION_CONTROL;
        if (filePath.rfind(".comp", filePath.size() - 1) != String::npos) return ShaderType::COMPUTE;
        if (filePath.rfind(".glsl", filePath.size() - 1) != String::npos) ASSERT_ERROR("Cannot determine the type of a shader file with extension of [.glsl]");

        return ShaderType::NONE;
    }

    shaderc_shader_kind Shader::GetShadercShaderType()
    {
        switch (shaderType)
        {
            case ShaderType::VERTEX:
                return shaderc_shader_kind::shaderc_glsl_vertex_shader;
            case ShaderType::FRAGMENT:
                return shaderc_shader_kind::shaderc_glsl_fragment_shader;
            case ShaderType::GEOMETRY:
                return shaderc_shader_kind::shaderc_glsl_geometry_shader;
            case ShaderType::TESSELATION_EVALUATION:
                return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
            case ShaderType::TESSELATION_CONTROL:
                return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
            case ShaderType::COMPUTE:
                return shaderc_shader_kind::shaderc_glsl_compute_shader;
            default:
                ASSERT_ERROR("Cannot determine the shaderc::shader_type of a [ShaderType::NONE]");
        }

        // A random value to eliminate compiler warning
        return shaderc_task_shader;
    }

    /* --- DESTRUCTORS --- */

    void Shader::Destroy()
    {
        if (precompiled) delete precompiledData;
        vkDestroyShaderModule(VK::GetLogicalDevice(), shaderModule, nullptr);
    }

    /* --- INCLUDER --- */

    Shader::Includer::Includer(const String givenFilePath)
            : filePath(givenFilePath)
    {

    }

    shaderc_include_result* Shader::Includer::GetInclude(const char* requestedSource, const shaderc_include_type type, const char* requestingSource, const size_t includeDepth)
    {
        // Get which file is requested to be included
        const std::string includeReference = std::string(requestedSource);

        // Gather its data
        auto fileData = File::ReadBinaryFile(File::RemoveFileNameFromPath(filePath) + includeReference);
        String includedContents = String(fileData.begin(), fileData.end());

        // Assign the poninters of the read data to a container
        auto container = new std::array<std::string, 2>();
        (*container)[0] = includeReference;
        (*container)[1] = includedContents;

        // Set the remaining data
        auto data = new shaderc_include_result;
        data->user_data = container;
        data->source_name = (*container)[0].data();
        data->source_name_length = (*container)[0].size();
        data->content = (*container)[1].data();
        data->content_length = (*container)[1].size();

        return data;
    }

    void Shader::Includer::ReleaseInclude(shaderc_include_result *data)
    {
        // Deallocate the container allocated in GetInclude()
        delete static_cast<std::array<std::string, 2>*>(data->user_data);
        delete data;
    }
}
