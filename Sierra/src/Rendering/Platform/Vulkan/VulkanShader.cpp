//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "VulkanShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */
    
    VulkanShader::VulkanShader(const VulkanDevice &device, const ShaderCreateInfo &createInfo)
        : Shader(createInfo), VulkanResource(createInfo.name), device(device)
    {
        // Set up module create info
        const ShaderFileHeader &fileHeader = *reinterpret_cast<const ShaderFileHeader*>(createInfo.memory.data());
        const VkShaderModuleCreateInfo shaderModuleCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = fileHeader.spvMemorySize,
            .pCode = reinterpret_cast<const uint32*>(createInfo.memory.data() + sizeof(ShaderFileHeader) + fileHeader.GetSpvOffset())
        };

        // Create shader module
        const VkResult result = device.GetFunctionTable().vkCreateShaderModule(device.GetLogicalDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create shader module for shader [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Set object name
        device.SetObjectName(shaderModule, VK_OBJECT_TYPE_SHADER_MODULE, GetName());
    }

    /* --- DESTRUCTOR --- */

    VulkanShader::~VulkanShader()
    {
        device.GetFunctionTable().vkDestroyShaderModule(device.GetLogicalDevice(), shaderModule, nullptr);
    }

    /* --- CONVERSIONS --- */

        VkShaderStageFlags VulkanShader::ShaderTypeToVkShaderStageFlags(const ShaderType shaderType)
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

}