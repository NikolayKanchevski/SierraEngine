//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "VulkanShader.h"

#include "VulkanResultHandler.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    VkShaderStageFlags ShaderTypeToVkShaderStageFlags(const ShaderType shaderType) noexcept
    {
        switch (shaderType)
        {
            case ShaderType::Vertex:        return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderType::Fragment:      return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderType::Compute:       return VK_SHADER_STAGE_COMPUTE_BIT;
            default:                        break;
        }

        return VK_SHADER_STAGE_ALL;
    }

    /* --- CONSTRUCTORS --- */
    
    VulkanShader::VulkanShader(const VulkanDevice& device, const ShaderCreateInfo& createInfo)
        : Shader(createInfo), device(device), name(createInfo.name)
    {
        // Set up module create info
        const ShaderFileHeader& fileHeader = *reinterpret_cast<const ShaderFileHeader*>(createInfo.memory.data());
        const VkShaderModuleCreateInfo shaderModuleCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = fileHeader.spvMemorySize,
            .pCode = reinterpret_cast<const uint32*>(createInfo.memory.data() + sizeof(ShaderFileHeader) + fileHeader.GetSpvOffset())
        };

        // Create shader module
        const VkResult result = device.GetFunctionTable().vkCreateShaderModule(device.GetVulkanDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create shader [{0}]", name));

        // Set object name
        device.SetResourceName(shaderModule, VK_OBJECT_TYPE_SHADER_MODULE, name);
    }

    /* --- DESTRUCTOR --- */

    VulkanShader::~VulkanShader() noexcept
    {
        device.GetFunctionTable().vkDestroyShaderModule(device.GetVulkanDevice(), shaderModule, nullptr);
    }

}