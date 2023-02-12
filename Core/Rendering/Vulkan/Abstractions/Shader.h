//
// Created by Nikolay Kanchevski on 18.12.22.
//

#pragma once

#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct ShaderCreateInfo
    {
        String filePath;
        ShaderType shaderType;

        const char* entryPoint = "main";
    };

    class Shader
    {
    public:
        /* --- CONSTRUCTORS --- */
        Shader(const ShaderCreateInfo &createInfo);
        static SharedPtr<Shader> Create(ShaderCreateInfo createInfo);

        /* --- SETTER METHODS --- */
        void Dispose();
        static void DisposePool();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkShaderModule GetVkShaderModule() const { return shaderModule; }
        [[nodiscard]] inline VkPipelineShaderStageCreateInfo& GetVkShaderStageInfo() { return shaderStageCreateInfo; }
        [[nodiscard]] inline std::vector<char>& GetShaderCode() { return shaderCode; }
        [[nodiscard]] inline ShaderType GetShaderType() const { return shaderType; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Shader);

    private:
        String filePath = "";
        ShaderType shaderType;

        std::vector<char> shaderCode;

        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};

        // filePath | Shader object
        inline static std::unordered_map<String, SharedPtr<Shader>> shaderPool;

    };

}