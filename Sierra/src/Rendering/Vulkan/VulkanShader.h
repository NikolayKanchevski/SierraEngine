//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "../Shader.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType shaderType) noexcept;

    class SIERRA_API VulkanShader final : public Shader, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanShader(const VulkanDevice& device, const ShaderCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkShaderModule GetVulkanShaderModule() const noexcept { return shaderModule; }

        /* --- COPY SEMANTICS --- */
        VulkanShader(const VulkanShader&) = delete;
        VulkanShader& operator=(const VulkanShader&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanShader(VulkanShader&&) noexcept = default;
        VulkanShader& operator=(VulkanShader&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanShader() noexcept override;

    private:
        const VulkanDevice* device = nullptr;

        VkShaderModule shaderModule = VK_NULL_HANDLE;

    };

}
