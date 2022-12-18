//
// Created by Nikolay Kanchevski on 18.12.22.
//

#include "Shader.h"

#include "../VulkanCore.h"
#include "../../../../Engine/Classes/File.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Shader::Shader(const ShaderCreateInfo &createInfo)
        : filePath(createInfo.filePath), shaderType(createInfo.shaderType), shaderCode(Engine::Classes::File::ReadFile(createInfo.filePath))
    {
        // Set module creation info
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = shaderCode.size();
        moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        // Create shader module
        VK_ASSERT(
            vkCreateShaderModule(VulkanCore::GetLogicalDevice(), &moduleCreateInfo, nullptr, &shaderModule),
            "Failed to create shader module for [" + createInfo.filePath + "]"
        );

        // Setup shader stage creation info
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.stage = (VkShaderStageFlagBits) shaderType;
        shaderStageCreateInfo.module = shaderModule;
        shaderStageCreateInfo.pName = "main";
    }

    std::shared_ptr<Shader> Shader::Create(ShaderCreateInfo createInfo)
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

    /* --- DESTRUCTORS --- */

    void Shader::Destroy()
    {
        vkDestroyShaderModule(VulkanCore::GetLogicalDevice(), shaderModule, nullptr);
    }

}
