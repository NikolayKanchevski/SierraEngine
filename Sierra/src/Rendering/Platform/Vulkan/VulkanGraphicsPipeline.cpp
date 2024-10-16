//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "VulkanGraphicsPipeline.h"

#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanDevice &device, const GraphicsPipelineCreateInfo &createInfo)
        : GraphicsPipeline(createInfo), VulkanResource(createInfo.name), device(device), layout(static_cast<VulkanPipelineLayout&>(*createInfo.layout))
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.layout->GetName());

        SR_ERROR_IF(createInfo.vertexShader->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.vertexShader->GetName());
        const VulkanShader &vulkanVertexShader = static_cast<VulkanShader&>(*createInfo.vertexShader);

        SR_ERROR_IF(createInfo.templateRenderPass->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.templateRenderPass->GetName());
        const VulkanRenderPass &vulkanRenderPass = static_cast<VulkanRenderPass&>(*createInfo.templateRenderPass);

        // Set up shader stages
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(1 + createInfo.fragmentShader.has_value());
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vulkanVertexShader.GetVulkanShaderModule();
        shaderStages[0].pName = "main";
        if (createInfo.fragmentShader.has_value())
        {
            SR_ERROR_IF(createInfo.fragmentShader->get()->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.vertexShader->GetName());
            const VulkanShader &vulkanFragmentShader = static_cast<VulkanShader&>(*createInfo.fragmentShader->get());
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
            vertexInputAttributes[i].binding = 0;
            vertexInputAttributes[i].location = i;
            vertexInputAttributes[i].offset = vertexDataSize;
            switch (*(createInfo.vertexInputs.begin() + i))
            {
                case VertexInput::Int8:          { vertexInputAttributes[i].format = VK_FORMAT_R8_SINT;    vertexDataSize += 1 * 1; break; }
                case VertexInput::UInt8:         { vertexInputAttributes[i].format = VK_FORMAT_R8_UINT;    vertexDataSize += 1 * 1; break; }
                case VertexInput::Norm8:         { vertexInputAttributes[i].format = VK_FORMAT_R8_SNORM;   vertexDataSize += 1 * 1; break; }
                case VertexInput::UNorm8:        { vertexInputAttributes[i].format = VK_FORMAT_R8_UNORM;   vertexDataSize += 1 * 1; break; }
                case VertexInput::Int16:         { vertexInputAttributes[i].format = VK_FORMAT_R16_SINT;   vertexDataSize += 1 * 2; break; }
                case VertexInput::UInt16:        { vertexInputAttributes[i].format = VK_FORMAT_R16_UINT;   vertexDataSize += 1 * 2; break; }
                case VertexInput::Norm16:        { vertexInputAttributes[i].format = VK_FORMAT_R16_SNORM;  vertexDataSize += 1 * 2; break; }
                case VertexInput::UNorm16:       { vertexInputAttributes[i].format = VK_FORMAT_R16_UNORM;  vertexDataSize += 1 * 2; break; }
                case VertexInput::Float16:       { vertexInputAttributes[i].format = VK_FORMAT_R16_SFLOAT; vertexDataSize += 1 * 4; break; }
                case VertexInput::Int32:         { vertexInputAttributes[i].format = VK_FORMAT_R32_SINT;   vertexDataSize += 1 * 4; break; }
                case VertexInput::UInt32:        { vertexInputAttributes[i].format = VK_FORMAT_R32_UINT;   vertexDataSize += 1 * 4; break; }
                case VertexInput::Float32:       { vertexInputAttributes[i].format = VK_FORMAT_R32_SFLOAT; vertexDataSize += 1 * 4; break; }

                case VertexInput::Int8_2D:       { vertexInputAttributes[i].format = VK_FORMAT_R8G8_SINT;     vertexDataSize += 2 * 1; break; }
                case VertexInput::UInt8_2D:      { vertexInputAttributes[i].format = VK_FORMAT_R8G8_UINT;     vertexDataSize += 2 * 1; break; }
                case VertexInput::Norm8_2D:      { vertexInputAttributes[i].format = VK_FORMAT_R8G8_SNORM;    vertexDataSize += 2 * 1; break; }
                case VertexInput::UNorm8_2D:     { vertexInputAttributes[i].format = VK_FORMAT_R8G8_UNORM;    vertexDataSize += 2 * 1; break; }
                case VertexInput::Int16_2D:      { vertexInputAttributes[i].format = VK_FORMAT_R16G16_SINT;   vertexDataSize += 2 * 2; break; }
                case VertexInput::UInt16_2D:     { vertexInputAttributes[i].format = VK_FORMAT_R16G16_UINT;   vertexDataSize += 2 * 2; break; }
                case VertexInput::Norm16_2D:     { vertexInputAttributes[i].format = VK_FORMAT_R16G16_SNORM;  vertexDataSize += 2 * 2; break; }
                case VertexInput::UNorm16_2D:    { vertexInputAttributes[i].format = VK_FORMAT_R16G16_UNORM;  vertexDataSize += 2 * 2; break; }
                case VertexInput::Float16_2D:    { vertexInputAttributes[i].format = VK_FORMAT_R16G16_SFLOAT; vertexDataSize += 2 * 2; break; }
                case VertexInput::Int32_2D:      { vertexInputAttributes[i].format = VK_FORMAT_R32G32_SINT;   vertexDataSize += 2 * 4; break; }
                case VertexInput::UInt32_2D:     { vertexInputAttributes[i].format = VK_FORMAT_R32G32_UINT;   vertexDataSize += 2 * 4; break; }
                case VertexInput::Float32_2D:    { vertexInputAttributes[i].format = VK_FORMAT_R32G32_SFLOAT; vertexDataSize += 2 * 4; break; }

                case VertexInput::Int8_3D:       { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8_SINT;      vertexDataSize += 3 * 1; break; }
                case VertexInput::UInt8_3D:      { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8_UINT;      vertexDataSize += 3 * 1; break; }
                case VertexInput::Norm8_3D:      { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8_SNORM;     vertexDataSize += 3 * 1; break; }
                case VertexInput::UNorm8_3D:     { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8_UNORM;     vertexDataSize += 3 * 1; break; }
                case VertexInput::Int16_3D:      { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16_SINT;   vertexDataSize += 3 * 2; break; }
                case VertexInput::UInt16_3D:     { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16_UINT;   vertexDataSize += 3 * 2; break; }
                case VertexInput::Norm16_3D:     { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16_SNORM;  vertexDataSize += 3 * 2; break; }
                case VertexInput::UNorm16_3D:    { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16_UNORM;  vertexDataSize += 3 * 2; break; }
                case VertexInput::Float16_3D:    { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16_SFLOAT; vertexDataSize += 3 * 2; break; }
                case VertexInput::Int32_3D:      { vertexInputAttributes[i].format = VK_FORMAT_R32G32B32_SINT;   vertexDataSize += 3 * 4; break; }
                case VertexInput::UInt32_3D:     { vertexInputAttributes[i].format = VK_FORMAT_R32G32B32_UINT;   vertexDataSize += 3 * 4; break; }
                case VertexInput::Float32_3D:    { vertexInputAttributes[i].format = VK_FORMAT_R32G32B32_SFLOAT; vertexDataSize += 3 * 4; break; }

                case VertexInput::Int8_4D:       { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8A8_SINT;       vertexDataSize += 4 * 1; break; }
                case VertexInput::UInt8_4D:      { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8A8_UINT;       vertexDataSize += 4 * 1; break; }
                case VertexInput::Norm8_4D:      { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8A8_SNORM;      vertexDataSize += 4 * 1; break; }
                case VertexInput::UNorm8_4D:     { vertexInputAttributes[i].format = VK_FORMAT_R8G8B8A8_UNORM;      vertexDataSize += 4 * 1; break; }
                case VertexInput::Int16_4D:      { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16A16_SINT;   vertexDataSize += 4 * 2; break; }
                case VertexInput::UInt16_4D:     { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16A16_UINT;   vertexDataSize += 4 * 2; break; }
                case VertexInput::Norm16_4D:     { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16A16_SNORM;  vertexDataSize += 4 * 2; break; }
                case VertexInput::UNorm16_4D:    { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16A16_UNORM;  vertexDataSize += 4 * 2; break; }
                case VertexInput::Float16_4D:    { vertexInputAttributes[i].format = VK_FORMAT_R16G16B16A16_SFLOAT; vertexDataSize += 4 * 2; break; }
                case VertexInput::Int32_4D:      { vertexInputAttributes[i].format = VK_FORMAT_R32G32B32A32_SINT;   vertexDataSize += 4 * 4; break; }
                case VertexInput::UInt32_4D:     { vertexInputAttributes[i].format = VK_FORMAT_R32G32B32A32_UINT;   vertexDataSize += 4 * 4; break; }
                case VertexInput::Float32_4D:    { vertexInputAttributes[i].format = VK_FORMAT_R32G32B32A32_SFLOAT; vertexDataSize += 4 * 4; break; }
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

        // Set up combined depth and stencil state
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = { };
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.depthTestEnable = vulkanRenderPass.HasDepthAttachment();
        depthStencilStateCreateInfo.depthWriteEnable = createInfo.depthMode == DepthMode::WriteDepth;
        depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.minDepthBounds = 0.0f;
        depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
        depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

        // Set up attachment color blending state
        VkPipelineColorBlendAttachmentState blendingAttachmentState = { };
        blendingAttachmentState.blendEnable = createInfo.blendMode != BlendMode::None;
        blendingAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendingAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendingAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendingAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendingAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates(createInfo.templateRenderPass->GetColorAttachmentCount(), blendingAttachmentState);

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
        graphicsPipelineCreateInfo.pDepthStencilState = vulkanRenderPass.HasDepthAttachment() ? &depthStencilStateCreateInfo : nullptr;
        graphicsPipelineCreateInfo.pColorBlendState = &blendingStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = layout.GetVulkanPipelineLayout();
        graphicsPipelineCreateInfo.renderPass = vulkanRenderPass.GetVulkanRenderPass();
        graphicsPipelineCreateInfo.subpass = createInfo.subpassIndex;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        // Create pipeline
        const VkResult result = device.GetFunctionTable().vkCreateGraphicsPipelines(device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create graphics pipeline [{0}]! Error code: {1}.", GetName(), result);

        // Set object name
        device.SetObjectName(pipeline, VK_OBJECT_TYPE_PIPELINE, GetName());
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
            case CullMode::Front:       return VK_CULL_MODE_FRONT_BIT;   // NOTE: These are inverted, because we are using negative framebuffer viewports to flip
            case CullMode::Back:        return VK_CULL_MODE_BACK_BIT;  //       output through the VK_KHR_MAINTENANCE_1 extension, which inverts culling
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
