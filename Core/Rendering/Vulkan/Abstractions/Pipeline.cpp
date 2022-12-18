//
// Created by Nikolay Kanchevski on 18.12.22.
//

#include "Pipeline.h"
#include "../VulkanCore.h"
#include "../../../../Engine/Structures/Vertex.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Pipeline::Pipeline(const PipelineCreateInfo &givenCreateInfo)
        : createInfo(givenCreateInfo)
    {
        CreatePipelineLayout();

        CreatePipeline();
    }

    std::unique_ptr<Pipeline> Pipeline::Create(PipelineCreateInfo createInfo)
    {
        return std::make_unique<Pipeline>(createInfo);
    }

    /* --- SETTER METHODS --- */

    void Pipeline::Bind(VkCommandBuffer givenCommandBuffer)
    {
        vkCmdBindPipeline(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vkPipeline);
    }

    void Pipeline::PushConstants(VkCommandBuffer givenCommandBuffer, const void *givenData)
    {
        ASSERT_ERROR_IF(pushConstantRange == nullptr, "Trying to push constants in pipeline that was created without support for them in mind");

        vkCmdPushConstants(
            givenCommandBuffer, vkPipelineLayout,
            pushConstantRange->stageFlags, pushConstantRange->offset,
            pushConstantRange->size, givenData
        );
    }

    void Pipeline::BindDescriptorSets(VkCommandBuffer givenCommandBuffer, const std::vector<VkDescriptorSet> descriptorSets)
    {
        vkCmdBindDescriptorSets(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
    }

    void Pipeline::OverloadShader(std::shared_ptr<Shader> newShader)
    {

        for (uint32_t i = createInfo.shaders.size(); i--;)
        {
            if (createInfo.shaders[i]->GetShaderType() == newShader->GetShaderType())
            {
                createInfo.shaders[i] = newShader;
                CreatePipeline();

                return;
            }
        }

        ASSERT_ERROR("Shader of type [" + std::to_string(newShader->GetShaderType()) + "] could not be overloaded because it is not present in the pipeline");
    }

    /* --- SETTER METHODS --- */

    void Pipeline::CreatePipelineLayout()
    {
        // Set pipeline layout creation info
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        if (createInfo.pushConstantRange != nullptr)
        {
            pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
            pipelineLayoutCreateInfo.pPushConstantRanges = createInfo.pushConstantRange;

            pushConstantRange = new VkPushConstantRange();
            pushConstantRange->size = createInfo.pushConstantRange->size;
            pushConstantRange->offset = createInfo.pushConstantRange->offset;
            pushConstantRange->stageFlags = createInfo.pushConstantRange->stageFlags;
        }

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        if (createInfo.descriptorSetLayout != nullptr)
        {
            descriptorSetLayouts = std::vector(createInfo.maxConcurrentFrames, createInfo.descriptorSetLayout->GetVulkanDescriptorSetLayout());

            pipelineLayoutCreateInfo.setLayoutCount = createInfo.maxConcurrentFrames;
            pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
        }

        // Create the pipeline layout
        VK_ASSERT(
            vkCreatePipelineLayout(VulkanCore::GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout),
            "Failed to create pipeline layout"
        );
    }

    void Pipeline::CreatePipeline()
    {
        if (alreadyCreated)
        {
            vkDestroyPipeline(VulkanCore::GetLogicalDevice(), vkPipeline, nullptr);
        }

        VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[createInfo.shaders.size()];
        for (uint32_t i = createInfo.shaders.size(); i--;)
        {
            shaderStages[i] = createInfo.shaders[i]->GetVkShaderStageInfo();
        }

        // Define the attributes to be sent to the shader
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

        // Set up for the "position" property
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        // Set up for the "normal" property
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        // Set up for the "textureCoordinates" property
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, textureCoordinates);

        // Set up binding description
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Set up how vertex data is sent
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Set up assembly info
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        // Set up viewport info
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = nullptr;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = nullptr;

        // Set up rasterization
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

        // Set up multi sampling
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.minSampleShading = 1.0f;
        multisampleStateCreateInfo.rasterizationSamples = (VkSampleCountFlagBits) createInfo.sampling;
        multisampleStateCreateInfo.pSampleMask = nullptr;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        if (createInfo.sampling > MSAAx1)
        {
            multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
            multisampleStateCreateInfo.minSampleShading = 0.2f;
        }

        // Set up color blending
        VkPipelineColorBlendAttachmentState blendingAttachmentState{};
        blendingAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendingAttachmentState.blendEnable = VK_FALSE;
        blendingAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendingAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendingAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendingAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendingAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        // Set up bindings
        VkPipelineColorBlendStateCreateInfo blendingStateCreateInfo{};
        blendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendingStateCreateInfo.logicOpEnable = VK_FALSE;
        blendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        blendingStateCreateInfo.attachmentCount = 1;
        blendingStateCreateInfo.pAttachments = &blendingAttachmentState;

        // Define dynamic states to use
        const std::vector<VkDynamicState> dynamicStates
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        // Set up dynamic states
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = 2;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        // Set up graphics pipeline creation info using all the modules created before
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = createInfo.shaders.size();
        graphicsPipelineCreateInfo.pStages = shaderStages;
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &blendingStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = vkPipelineLayout;
        graphicsPipelineCreateInfo.renderPass = createInfo.renderPass->GetVulkanRenderPass();
        graphicsPipelineCreateInfo.subpass = 0;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        if (createInfo.createDepthBuffer)
        {
            VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
            depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
            depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
            depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
            depthStencilStateCreateInfo.minDepthBounds = 0.0f;
            depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
            depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

            graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        }

        // Create the graphics pipeline
        VK_ASSERT(
            vkCreateGraphicsPipelines(VulkanCore::GetLogicalDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipeline),
            "Failed to create graphics pipeline"
        );

        alreadyCreated = true;
    }

    /* --- DESTRUCTOR --- */

    void Pipeline::Destroy()
    {
        if (pushConstantRange != nullptr) delete pushConstantRange;

        vkDestroyPipeline(VulkanCore::GetLogicalDevice(), vkPipeline, nullptr);
        vkDestroyPipelineLayout(VulkanCore::GetLogicalDevice(), vkPipelineLayout, nullptr);
    }
}
