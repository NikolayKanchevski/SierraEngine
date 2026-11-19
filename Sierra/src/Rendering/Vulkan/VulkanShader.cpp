//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "VulkanShader.h"

#include "VulkanErrorHandler.h"

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
    
    VulkanShader::VulkanShader(const VulkanDevice& givenDevice, const ShaderCreateInfo& createInfo)
        : Shader(createInfo), name(createInfo.name), device(&givenDevice)
    {
        SR_THROW_IF(createInfo.code.empty(), InvalidValueError(SR_FORMAT("Cannot create shader [{0}], as specified SPIR-V code must not be empty", createInfo.name)));
        SR_THROW_IF(createInfo.code.size() % sizeof(uint32) != 0, InvalidValueError(SR_FORMAT("Cannot create shader [{0}], as specified SPIR-V code's memory size [{1}] is not a multiple of [sizeof(uint32) = 4]", createInfo.name, createInfo.code.size(), sizeof(uint32))));

        // Set up module create info
        const VkShaderModuleCreateInfo shaderModuleCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = createInfo.code.size(),
            .pCode = reinterpret_cast<const uint32*>(createInfo.code.data())
        };

        // Create shader module
        const VkResult result = device->GetFunctionTable().vkCreateShaderModule(device->GetVulkanDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create shader [{0}]", createInfo.name));

        // Set object name
        device->SetResourceName(shaderModule, VK_OBJECT_TYPE_SHADER_MODULE, createInfo.name);
    }

    /* --- DESTRUCTOR --- */

    VulkanShader::~VulkanShader() noexcept
    {
        device->GetFunctionTable().vkDestroyShaderModule(device->GetVulkanDevice(), shaderModule, nullptr);
    }

}