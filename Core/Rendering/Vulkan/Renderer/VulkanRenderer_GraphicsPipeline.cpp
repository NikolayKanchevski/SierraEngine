//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"
#include "../../../../Engine/Classes/File.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateGraphicsPipeline()
    {
        // Create shader modules out of the read shader
        VkShaderModule vertShaderModule = VulkanUtilities::CreateShaderModule(VulkanCore::GetDescriptorIndexingSupported() ? "Shaders/vertex_shader_bindless.vert.spv" : "Shaders/vertex_shader.vert.spv");
        VkShaderModule fragShaderModule = VulkanUtilities::CreateShaderModule(VulkanCore::GetDescriptorIndexingSupported() ? "Shaders/fragment_shader_bindless.frag.spv" : "Shaders/fragment_shader.frag.spv");

        // Set vertex shader properties
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        // Set fragment shader properties
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        const std::vector<VkPipelineShaderStageCreateInfo> shaderStages { vertShaderStageInfo, fragShaderStageInfo };

        // Set up binding description
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

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

        // Determine the polygon mode depending on "renderingMode"
        switch (renderingMode)
        {
            case Fill:
                rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
                break;
            case Wireframe:
                rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
                break;
        }

        // Set up multi sampling
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.minSampleShading = 1.0f;
        multisampleStateCreateInfo.rasterizationSamples = msaaSampleCount;
        multisampleStateCreateInfo.pSampleMask = nullptr;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        if (this->msaaSamplingEnabled)
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

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.minDepthBounds = 0.0f;
        depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
        depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

        const std::vector<VkDescriptorSetLayout> descriptorSetLayouts(maxConcurrentFrames, descriptorSetLayout->GetVulkanDescriptorSetLayout());

        const std::vector<VkPushConstantRange> pushConstantRanges { this->pushConstantRange };

        // Set pipeline layout creation info
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = maxConcurrentFrames;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRanges.size();
        pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

        // Create the pipeline layout
        VK_ASSERT(
            vkCreatePipelineLayout(device->GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &graphicsPipelineLayout),
            "Failed to create pipeline layout"
        );

        // Set up graphics pipeline creation info using all the modules created before
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = shaderStages.size();
        graphicsPipelineCreateInfo.pStages = shaderStages.data();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &blendingStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = graphicsPipelineLayout;
        graphicsPipelineCreateInfo.renderPass = this->offscreenRenderer->GetRenderPass()->GetVulkanRenderPass();
        graphicsPipelineCreateInfo.subpass = 0;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        // Create the graphics pipeline
        VK_ASSERT(
            vkCreateGraphicsPipelines(device->GetLogicalDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline),
            "Failed to create graphics pipeline"
        );

        // Destroy the made shader modules
        vkDestroyShaderModule(device->GetLogicalDevice(), vertShaderModule, nullptr);
        vkDestroyShaderModule(device->GetLogicalDevice(), fragShaderModule, nullptr);
    }

}