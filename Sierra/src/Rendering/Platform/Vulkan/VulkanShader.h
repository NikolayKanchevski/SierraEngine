//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "../../Shader.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanShader final : public Shader, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanShader(const VulkanDevice &device, const ShaderCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkShaderModule GetVulkanShaderModule() const { return shaderModule; }

        /* --- DESTRUCTOR --- */
        ~VulkanShader() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType shaderType);

    private:
        const VulkanDevice &device;

        VkShaderModule shaderModule = VK_NULL_HANDLE;

    };

}
