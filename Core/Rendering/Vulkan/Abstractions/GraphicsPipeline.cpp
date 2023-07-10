//
// Created by Nikolay Kanchevski on 18.05.23.
//

#include "GraphicsPipeline.h"

#include "../VK.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo)
        : Pipeline({ .maxConcurrentFrames = VK::GetDevice()->GetMaxConcurrentFramesCount(), .shaders = std::move(createInfo.shaders), .shaderInfo = std::move(createInfo.shaderInfo) }),
          sampling(createInfo.sampling), frontFace(createInfo.frontFace), cullMode(createInfo.cullMode), shadingType(createInfo.shadingType), dynamicRenderingInfo(createInfo.dynamicRenderingInfo), renderPassInfo(createInfo.renderPassInfo), depthBiasEnabled(createInfo.enableDepthBias)
    {
        CreatePipeline();
    }

    UniquePtr<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineCreateInfo &createInfo)
    {
        return std::make_unique<GraphicsPipeline>(createInfo);
    }

    void GraphicsPipeline::CreatePipeline()
    {
        // Check if recreating
        if (vkPipeline != nullptr)
        {
            VK::GetDevice()->WaitUntilIdle();
            vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
        }

        // Set bind point
        bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        // Define shader stages for pipeline
        VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[shaders.size()];

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Define the attributes to be sent to the shader
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        uint i = 0;
        for (const auto &[shaderType, shader] : shaders)
        {
            shaderStages[i] = *shader->shaderStageCreateInfo;
            if (shaderType == ShaderType::VERTEX)
            {
                if (usesPrecompiledShaders)
                {
                    uint lastOffset = 0;
                    auto &reflectionData = shader->GetPrecompiledData()->reflectionData;
                    if (reflectionData.vertexAttributes != nullptr)
                    {
                        // Calculate attributes' offset and assign location & format
                        attributeDescriptions.resize(reflectionData.vertexAttributes->size());
                        for (uint j = 0; j < attributeDescriptions.size(); j++)
                        {
                            const auto &attributeInfo = (*reflectionData.vertexAttributes)[j];
                            attributeDescriptions[j].binding = 0;
                            attributeDescriptions[j].location = attributeInfo.location;
                            attributeDescriptions[j].format = static_cast<VkFormat>(attributeInfo.vertexAttributeType);
                            attributeDescriptions[j].offset = lastOffset;

                            lastOffset += GetVertexAttributeTypeSize(attributeInfo.vertexAttributeType);
                        }

                        // Update stride
                        bindingDescription.stride = lastOffset;
                    }
                }
                else
                {
                    if (shader->vertexAttributeCount > 0)
                    {
                        // Calculate attributes' offset and assign location & format
                        attributeDescriptions = std::vector<VkVertexInputAttributeDescription>(shader->vertexAttributes, shader->vertexAttributes + shader->vertexAttributeCount);

                        // Update stride
                        bindingDescription.stride = attributeDescriptions.back().offset + GetVertexAttributeTypeSize(static_cast<VertexAttributeType>(attributeDescriptions.back().format));
                    }
                }
            }

            i++;
        }

        // Set up how vertex data is sent
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        if (!attributeDescriptions.empty())
        {
            vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
            vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
            vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        }

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
        rasterizationStateCreateInfo.cullMode = static_cast<VkCullModeFlags>(cullMode);
        rasterizationStateCreateInfo.polygonMode = static_cast<VkPolygonMode>(shadingType);
        rasterizationStateCreateInfo.frontFace = static_cast<VkFrontFace>(frontFace);
        rasterizationStateCreateInfo.depthBiasEnable = depthBiasEnabled;

        // Set up multisampling
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.minSampleShading = 1.0f;
        multisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(Sampling::MSAAx1);
        multisampleStateCreateInfo.pSampleMask = nullptr;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
        if (sampling > Sampling::MSAAx1)
        {
            if (VK::GetDevice()->GetHighestMultisampling() < sampling)
            {
                ASSERT_WARNING_FORMATTED("Requested a sampling of [{0}] for pipeline, however, highest sampling supported is [{1}]! Sampling was automatically lowered to maximum", VK_TO_STRING(sampling, SampleCountFlagBits), VK_TO_STRING(VK::GetDevice()->GetHighestMultisampling(), SampleCountFlagBits));
                sampling = VK::GetDevice()->GetHighestMultisampling();
            }
            else
            {
                multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
                multisampleStateCreateInfo.minSampleShading = 0.2f;
            }
        }
        multisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(sampling);

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

        ASSERT_ERROR_IF((!renderPassInfo.has_value() && !dynamicRenderingInfo.has_value()) || (renderPassInfo.has_value() && dynamicRenderingInfo.has_value()), "When creating graphics pipelines either [renderPassInfo] or [dynamicRenderingInfo] must contain value and, but not both");

        // Check what technology is used
        bool usesRenderPasses = renderPassInfo.has_value();
        uint colorAttachmentCount = usesRenderPasses ? renderPassInfo.value().renderPass->GetColorAttachmentCount() : dynamicRenderingInfo.value().dynamicRenderer->GetColorAttachmentCount();

        // Set up color attachment states
        std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates(colorAttachmentCount, blendingAttachmentState);

        // Set up blending state
        VkPipelineColorBlendStateCreateInfo blendingStateCreateInfo{};
        blendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendingStateCreateInfo.logicOpEnable = VK_FALSE;
        blendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        blendingStateCreateInfo.attachmentCount = colorAttachmentStates.size();
        blendingStateCreateInfo.pAttachments = colorAttachmentStates.data();

        VkPipelineRenderingCreateInfoKHR* dynamicRenderingInfoKHR = nullptr;
        if (!usesRenderPasses)
        {
            dynamicRenderingInfoKHR = new VkPipelineRenderingCreateInfoKHR();
            dynamicRenderingInfoKHR->sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
            dynamicRenderingInfoKHR->colorAttachmentCount = colorAttachmentCount;
            dynamicRenderingInfoKHR->pColorAttachmentFormats = dynamicRenderingInfo.value().dynamicRenderer->GetColorAttachmentFormats();
            dynamicRenderingInfoKHR->depthAttachmentFormat = dynamicRenderingInfo.value().dynamicRenderer->GetDepthStencilAttachmentFormat();
            dynamicRenderingInfoKHR->stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
            dynamicRenderingInfoKHR->pNext = nullptr;
        }

        // Define dynamic states to use
        std::vector<VkDynamicState> dynamicStates
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        if (depthBiasEnabled)
        {
            dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
        }

        // Set up dynamic states
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        // Set up graphics pipeline creation info using all the modules created before
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = shaders.size();
        graphicsPipelineCreateInfo.pStages = shaderStages;
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &blendingStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = vkPipelineLayout;
        graphicsPipelineCreateInfo.subpass = 0;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        if (usesRenderPasses)
        {
            graphicsPipelineCreateInfo.subpass = renderPassInfo.value().subpass;
            graphicsPipelineCreateInfo.renderPass = renderPassInfo.value().renderPass->GetVulkanRenderPass();
        }
        else
        {
            graphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
            graphicsPipelineCreateInfo.pNext = dynamicRenderingInfoKHR;
        }

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
        if ((usesRenderPasses && renderPassInfo.value().renderPass->HasDepthAttachment()) || (!usesRenderPasses && dynamicRenderingInfo.value().dynamicRenderer.get()->GetDepthStencilAttachmentFormat() != VK_FORMAT_UNDEFINED))
        {
            depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
            depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
            depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
            depthStencilStateCreateInfo.minDepthBounds = 0.0f;
            depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
            depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

            graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        }

        // Create the graphics pipeline
        VK_ASSERT(
            vkCreateGraphicsPipelines(VK::GetLogicalDevice(), pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipeline),
            "Failed to create graphics pipeline"
        );

        delete[](shaderStages);
        delete(dynamicRenderingInfoKHR);
    }

    /* --- POLLING METHODS --- */

    void GraphicsPipeline::Draw(const UniquePtr<CommandBuffer> &commandBuffer, const uint vertexCount)
    {
        BindResources(commandBuffer);
        vkCmdDraw(commandBuffer->GetVulkanCommandBuffer(), vertexCount, 1, 0, 0);
    }

    void GraphicsPipeline::DrawMesh(const UniquePtr<CommandBuffer> &commandBuffer, const SharedPtr<Engine::Mesh> &mesh)
    {
        BindResources(commandBuffer);
        constexpr static VkDeviceSize offsets[] { 0 };

        VkBuffer vertexBuffer = mesh->GetVertexBuffer()->GetVulkanBuffer();
        vkCmdBindVertexBuffers(commandBuffer->GetVulkanCommandBuffer(), 0, 1, &vertexBuffer, offsets);

        VkBuffer indexBuffer = mesh->GetIndexBuffer()->GetVulkanBuffer();
        vkCmdBindIndexBuffer(commandBuffer->GetVulkanCommandBuffer(), indexBuffer, 0, VK_INDEX_BUFFER_TYPE);

        vkCmdDrawIndexed(commandBuffer->GetVulkanCommandBuffer(), mesh->GetIndexCount(), 1, 0, 0, 0);
    }

    /* --- SETTER METHODS --- */

    void GraphicsPipeline::SetFrontFace(const FrontFace givenFrontFace)
    {
        ASSERT_ERROR_IF(bound, "Cannot modify front face of a pipeline that is bound");
        frontFace = givenFrontFace;
        CreatePipeline();
    }

    void GraphicsPipeline::SetCullMode(const CullMode givenCullMode)
    {
        ASSERT_ERROR_IF(bound, "Cannot modify cull mode of a pipeline that is bound");
        cullMode = givenCullMode;
        CreatePipeline();
    }

    void GraphicsPipeline::SetShadingType(const ShadingType givenShadingType)
    {
        ASSERT_ERROR_IF(bound, "Cannot modify shading type of a pipeline that is bound");
        shadingType = givenShadingType;
        CreatePipeline();
    }

    /* --- DESTRUCTOR --- */

    void GraphicsPipeline::Destroy()
    {
        Pipeline::Destroy();
    }

}
