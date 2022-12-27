//
// Created by Nikolay Kanchevski on 18.12.22.
//

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct ShaderCreateInfo
    {
        std::string filePath;
        ShaderType shaderType;

        const char* entryPoint = "main";
    };

    class Shader
    {
    public:
        /* --- CONSTRUCTORS --- */
        Shader(const ShaderCreateInfo &createInfo);
        static std::shared_ptr<Shader> Create(ShaderCreateInfo createInfo);

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
        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;

    private:
        std::string filePath = "";
        ShaderType shaderType;

        std::vector<char> shaderCode;

        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};

        // filePath | Shader object
        inline static std::unordered_map<std::string, std::shared_ptr<Shader>> shaderPool;

    };

}