//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "VulkanGraphicsPipeline.h"

#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanDevice &device, const GraphicsPipelineCreateInfo &createInfo)
        : GraphicsPipeline(createInfo), VulkanPipeline(device, { .name = createInfo.name, .layout = createInfo.layout, .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS })
    {
        SR_ERROR_IF(createInfo.vertexShader->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.vertexShader->GetName());
        const VulkanShader &vulkanVertexShader = static_cast<VulkanShader&>(*createInfo.vertexShader);

        SR_ERROR_IF(createInfo.renderPass->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.renderPass->GetName());
        const VulkanRenderPass &vulkanRenderPass = static_cast<VulkanRenderPass&>(*createInfo.renderPass);

        // Set up shader stages
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(1 + createInfo.fragmentShader.has_value());
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vulkanVertexShader.GetVulkanShaderModule();
        shaderStages[0].pName = "main";
        if (createInfo.fragmentShader.has_value())
        {
            SR_ERROR_IF(createInfo.fragmentShader.value().get()->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.vertexShader->GetName());
            const VulkanShader &vulkanFragmentShader = static_cast<VulkanShader&>(*createInfo.fragmentShader.value().get());
            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].module = vulkanFragmentShader.GetVulkanShaderModule();
            shaderStages[1].pName = "main";
        }

        // Set up vertex attributes
        uint32 vertexDataSize = 0;
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes(createInfo.vertexInputs.size());
        for (uint32 i = 0; i < createInfo.vertexInputs.size(); i++)
        {
            vertexInputAttributes[i].location = 0;
            vertexInputAttributes[i].binding = i;
            vertexInputAttributes[i].offset = vertexDataSize;
            switch (*(createInfo.vertexInputs.begin() + i))
            {
                case VertexInput::Float:
                {
                    vertexInputAttributes[i].format = VK_FORMAT_R32_SFLOAT;
                    vertexDataSize += sizeof(float32) * 1;
                    break;
                }
                case VertexInput::Float2:
                {
                    vertexInputAttributes[i].format = VK_FORMAT_R32G32_SFLOAT;
                    vertexDataSize += sizeof(float32) * 2;
                    break;
                }
                case VertexInput::Float3:
                {
                    vertexInputAttributes[i].format = VK_FORMAT_R32G32B32_SFLOAT;
                    vertexDataSize += sizeof(float32) * 3;
                    break;
                }
                case VertexInput::Float4:
                {
                    vertexInputAttributes[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                    vertexDataSize += sizeof(float32) * 4;
                    break;
                }
            }
        }

        // Set up vertex input binding
        VkVertexInputBindingDescription vertexInputBinding = { };
        vertexInputBinding.binding = 0;
        vertexInputBinding.stride = vertexDataSize;
        vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Set up how vertex data is sent
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = { };
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = createInfo.vertexInputs.size() > 0;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBinding;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributes.size());
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributes.data();

        // Set up input assembly state
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = { };
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        // Set up viewport state
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = { };
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = nullptr;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = nullptr;

        // Set up rasterization state
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = { };
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = CullModeToVkCullMode(createInfo.cullMode);
        rasterizationStateCreateInfo.polygonMode = ShadeModeToVkPolygonMode(createInfo.shadeMode);
        rasterizationStateCreateInfo.frontFace = FrontFaceModeToVkFrontFace(createInfo.frontFaceMode);
        rasterizationStateCreateInfo.depthBiasEnable = false;

        // Set up multisampling state
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = { };
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.rasterizationSamples = VulkanImage::ImageSamplingToVkSampleCountFlags(createInfo.sampling);
        multisampleStateCreateInfo.sampleShadingEnable = createInfo.sampling != ImageSampling::x1; // Support of sampling here should not matter, as long as render pass' attachments are of a valid one
        multisampleStateCreateInfo.minSampleShading = createInfo.sampling != ImageSampling::x1 ? 0.2f : 1.0f;
        multisampleStateCreateInfo.pSampleMask = nullptr;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        // Set up attachment color blending state
        VkPipelineColorBlendAttachmentState blendingAttachmentState = { };
        blendingAttachmentState.blendEnable = VK_FALSE;
        blendingAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendingAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendingAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendingAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendingAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates(createInfo.renderPass->GetColorAttachmentCount(), blendingAttachmentState);

        // Set up color blending state
        VkPipelineColorBlendStateCreateInfo blendingStateCreateInfo = { };
        blendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendingStateCreateInfo.logicOpEnable = VK_FALSE;
        blendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        blendingStateCreateInfo.attachmentCount = static_cast<uint32>(colorAttachmentStates.size());
        blendingStateCreateInfo.pAttachments = colorAttachmentStates.data();

        // Define dynamic states to use
        constexpr std::array<VkDynamicState, 2> dynamicStates
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        // Set up dynamic states
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { };
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32>(dynamicStates.size());
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        // Set up graphics pipeline creation info using all the modules created before
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = { };
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(shaderStages.size());
        graphicsPipelineCreateInfo.pStages = shaderStages.data();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &blendingStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = pipelineLayout.GetVulkanPipelineLayout();
        graphicsPipelineCreateInfo.renderPass = vulkanRenderPass.GetVulkanRenderPass();
        graphicsPipelineCreateInfo.subpass = createInfo.subpassIndex;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        // Create pipeline
        const VkResult result = device.GetFunctionTable().vkCreateGraphicsPipelines(device.GetLogicalDevice(), nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create graphics pipeline [{0}]! Error code: {1}.");

        // Set object name
        device.SetObjectName(pipeline, VK_OBJECT_TYPE_PIPELINE, GetName());
    }

    /* --- POLLING METHODS --- */

    void VulkanGraphicsPipeline::BindVertexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &vertexBuffer, const uint64 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind vertex buffer [{0}] to graphics pipeline [{1}] using command buffer [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", vertexBuffer->GetName(), GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(vertexBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind vertex buffer [{0}] to graphics pipeline [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", vertexBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanVertexBuffer = static_cast<const VulkanBuffer&>(*vertexBuffer);

        SR_ERROR_IF(offset > vertexBuffer->GetMemorySize(), "[Vulkan]: Cannot bind vertex buffer [{0}] to graphics pipeline [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", vertexBuffer->GetName(), GetName(), offset, vertexBuffer->GetMemorySize());

        const VkBuffer vkBuffer = vulkanVertexBuffer.GetVulkanBuffer();
        device.GetFunctionTable().vkCmdBindVertexBuffers(vulkanCommandBuffer.GetVulkanCommandBuffer(), 0, 1, &vkBuffer, &offset);
    }

    void VulkanGraphicsPipeline::BindIndexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &indexBuffer, const uint64 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind index buffer [{0}] to graphics pipeline [{1}] using command buffer [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", indexBuffer->GetName(), GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind index buffer [{0}] to graphics pipeline [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", indexBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanIndexBuffer = static_cast<const VulkanBuffer&>(*indexBuffer);

        SR_ERROR_IF(offset > indexBuffer->GetMemorySize(), "[Vulkan]: Cannot bind index buffer [{0}] to graphics pipeline [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", indexBuffer->GetName(), GetName(), offset, indexBuffer->GetMemorySize());
        device.GetFunctionTable().vkCmdBindIndexBuffer(vulkanCommandBuffer.GetVulkanCommandBuffer(), vulkanIndexBuffer.GetVulkanBuffer(), offset, VK_INDEX_TYPE_UINT32); // 32-bit indices are required due to Metal shaders enforcing it
    }

    void VulkanGraphicsPipeline::Draw(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 vertexCount) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot issue a draw call on graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        BindResources(vulkanCommandBuffer);
        device.GetFunctionTable().vkCmdDraw(vulkanCommandBuffer.GetVulkanCommandBuffer(), vertexCount, 1, 0, 0);
    }

    void VulkanGraphicsPipeline::DrawIndexed(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 indexCount, const uint64 indexOffset, const uint64 vertexOffset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot issue an indexed draw call on graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        BindResources(vulkanCommandBuffer);
        device.GetFunctionTable().vkCmdDrawIndexed(vulkanCommandBuffer.GetVulkanCommandBuffer(), indexCount, 1, indexOffset, static_cast<int32>(vertexOffset), 0);
    }

    /* --- DESTRUCTOR --- */

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        device.GetFunctionTable().vkDestroyPipeline(device.GetLogicalDevice(), pipeline, nullptr);
    }

    /* --- CONVERSIONS --- */

    VkCullModeFlags VulkanGraphicsPipeline::CullModeToVkCullMode(const CullMode cullMode)
    {
        switch (cullMode)
        {
            case CullMode::None:        return VK_CULL_MODE_NONE;
            case CullMode::Front:       return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back:        return VK_CULL_MODE_BACK_BIT;
        }

        return VK_CULL_MODE_NONE;
    }

    VkPolygonMode VulkanGraphicsPipeline::ShadeModeToVkPolygonMode(const ShadeMode shadeMode)
    {
        switch (shadeMode)
        {
            case ShadeMode::Fill:           return VK_POLYGON_MODE_FILL;
            case ShadeMode::Wireframe:      return VK_POLYGON_MODE_LINE;
        }
        
        return VK_POLYGON_MODE_FILL;
    }

    VkFrontFace VulkanGraphicsPipeline::FrontFaceModeToVkFrontFace(const FrontFaceMode frontFaceMode)
    {
        switch (frontFaceMode)
        {
            case FrontFaceMode::Clockwise:              return VK_FRONT_FACE_CLOCKWISE;
            case FrontFaceMode::CounterClockwise:       return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }

        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

}
