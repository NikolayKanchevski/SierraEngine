//
// Created by Nikolay Kanchevski on 9.01.24.
//

#include "VulkanPipelineLayout.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice &device, const PipelineLayoutCreateInfo &createInfo)
        : PipelineLayout(createInfo), VulkanResource(createInfo.name), device(device), bindings(std::vector<PipelineBinding>(createInfo.bindings.begin(), createInfo.bindings.end())), pushConstantSize(createInfo.pushConstantSize)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME), "[Vulkan]: Cannot create pipeline layout [{0}], as the provided device [{1}] does not support the {2} extension!", GetName(), device.GetName(), VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);

        // Create descriptor set layout
        if (createInfo.bindings.size() > 0)
        {
            std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(createInfo.bindings.size());
            for (uint32 i = 0; i < createInfo.bindings.size(); i++)
            {
                const PipelineBinding &binding = *(createInfo.bindings.begin() + i);
                SR_ERROR_IF(binding.type == PipelineBindingType::Undefined, "[Vulkan]: Cannot create pipeline layout [{0}], as binding [{1}] must not be of type [PipelineBindingType::Undefined]!", GetName(), i);

                descriptorSetLayoutBindings[i].binding = i;
                descriptorSetLayoutBindings[i].descriptorType = PipelineBindingTypeToVkDescriptorType(binding.type);
                descriptorSetLayoutBindings[i].descriptorCount = binding.arraySize;
                descriptorSetLayoutBindings[i].stageFlags = VK_SHADER_STAGE_ALL;
                descriptorSetLayoutBindings[i].pImmutableSamplers = nullptr;
            }

            // Set up  layout create info
            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { };
            descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
            descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.size());
            descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

            // Create descriptor set layout
            const VkResult result = device.GetFunctionTable().vkCreateDescriptorSetLayout(device.GetLogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan] Could not create descriptor set layout of pipeline layout [{0}]! Error code: {1}.", GetName(), result);
        }

        // Set up push constant range
        VkPushConstantRange pushConstantRange = { };
        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRange.offset = 0;
        pushConstantRange.size = createInfo.pushConstantSize;

        // Set up layout create info
        VkPipelineLayoutCreateInfo layoutCreateInfo = { };
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = descriptorSetLayout != VK_NULL_HANDLE;
        layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        layoutCreateInfo.pushConstantRangeCount = createInfo.pushConstantSize > 0;
        layoutCreateInfo.pPushConstantRanges = &pushConstantRange;

        // Create pipeline layout
        const VkResult result = device.GetFunctionTable().vkCreatePipelineLayout(device.GetLogicalDevice(), &layoutCreateInfo, nullptr, &pipelineLayout);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create pipeline layout [{0}]! Error code: {1}.", GetName(), result);

        // Set object name
        device.SetObjectName(pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, GetName());
    }

    /* --- DESTRUCTOR --- */

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        device.GetFunctionTable().vkDestroyPipelineLayout(device.GetLogicalDevice(), pipelineLayout, nullptr);
        if (descriptorSetLayout != VK_NULL_HANDLE) device.GetFunctionTable().vkDestroyDescriptorSetLayout(device.GetLogicalDevice(), descriptorSetLayout, nullptr);
    }

    /* --- CONVERSIONS --- */

    VkDescriptorType VulkanPipelineLayout::PipelineBindingTypeToVkDescriptorType(const PipelineBindingType bindingType)
    {
        switch (bindingType)
        {
            case PipelineBindingType::Image:                return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case PipelineBindingType::Texture:              return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case PipelineBindingType::InputAttachment:      return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            case PipelineBindingType::UniformBuffer:        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case PipelineBindingType::StorageBuffer:        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case PipelineBindingType::Undefined:            break;
        }

        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }

}