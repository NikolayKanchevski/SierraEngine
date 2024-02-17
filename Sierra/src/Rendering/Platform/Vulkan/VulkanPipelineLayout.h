//
// Created by Nikolay Kanchevski on 9.01.24.
//

#pragma once

#include "../../PipelineLayout.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanPipelineLayout final : public PipelineLayout, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanPipelineLayout(const VulkanDevice &device, const PipelineLayoutCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return descriptorSetLayout; }
        [[nodiscard]] inline VkPipelineLayout GetVulkanPipelineLayout() const { return pipelineLayout; }

        [[nodiscard]] inline uint16 GetPushConstantSize() const { return pushConstantSize; }
        [[nodiscard]] inline const std::vector<PipelineBinding>& GetBindings() const { return bindings; }

        /* --- DESTRUCTOR --- */
        ~VulkanPipelineLayout() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkDescriptorType PipelineBindingTypeToVkDescriptorType(PipelineBindingType bindingType);

    private:
        const VulkanDevice &device;

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        uint16 pushConstantSize = 0;
        std::vector<PipelineBinding> bindings;

    };

}
