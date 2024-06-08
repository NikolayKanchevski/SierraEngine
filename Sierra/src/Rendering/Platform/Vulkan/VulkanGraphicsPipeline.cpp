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
        : GraphicsPipeline(createInfo), VulkanResource(createInfo.name), device(device), pushConstantSize(createInfo.pushConstantSize)
    {
        SR_ERROR_IF(createInfo.vertexShader.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.vertexShader.GetName());
        const VulkanShader &vulkanVertexShader = static_cast<const VulkanShader&>(createInfo.vertexShader);

        SR_ERROR_IF(createInfo.templateRenderPass.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.templateRenderPass.GetName());
        const VulkanRenderPass &vulkanRenderPass = static_cast<const VulkanRenderPass&>(createInfo.templateRenderPass);

        // Set up shader stages
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(1 + (createInfo.fragmentShader != nullptr));
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vulkanVertexShader.GetVulkanShaderModule();
        shaderStages[0].pName = "main";
        if (createInfo.fragmentShader != nullptr)
        {
            SR_ERROR_IF(createInfo.fragmentShader->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.fragmentShader->GetName());
            const VulkanShader &vulkanFragmentShader = static_cast<const VulkanShader&>(*createInfo.fragmentShader);
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
            switch (createInfo.vertexInputs[i])
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
        const VkVertexInputBindingDescription vertexInputBinding
        {
            .binding = 0,
            .stride = vertexDataSize,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        // Set up how vertex data is sent
        const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = !createInfo.vertexInputs.empty(),
            .pVertexBindingDescriptions = &vertexInputBinding,
            .vertexAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributes.size()),
            .pVertexAttributeDescriptions = vertexInputAttributes.data()
        };

        // Set up input assembly state
        constexpr VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        // Set up viewport state
        constexpr VkPipelineViewportStateCreateInfo viewportStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr
        };

        // Set up rasterization state
        const VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = ShadeModeToVkPolygonMode(createInfo.shadeMode),
            .cullMode = CullModeToVkCullMode(createInfo.cullMode),
            .frontFace = FrontFaceModeToVkFrontFace(createInfo.frontFaceMode),
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f
        };

        // Set up multisampling state
        const VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VulkanImage::ImageSamplingToVkSampleCountFlags(createInfo.sampling),
            .sampleShadingEnable = createInfo.sampling != ImageSampling::x1, // Support of sampling here should not matter, as long as render pass' attachments are of a valid one
            .minSampleShading = createInfo.sampling != ImageSampling::x1 ? 0.2f : 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        // Set up combined depth and stencil state
        const VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = vulkanRenderPass.HasDepthAttachment(),
            .depthWriteEnable = createInfo.depthMode == DepthMode::WriteDepth,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f
        };

        // Set up attachment color blending state
        const VkPipelineColorBlendAttachmentState blendingAttachmentState
        {
            .blendEnable = createInfo.blendMode != BlendMode::None,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };
        std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates(createInfo.templateRenderPass.GetColorAttachmentCount(), blendingAttachmentState);

        // Set up color blending state
        const VkPipelineColorBlendStateCreateInfo blendingStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = static_cast<uint32>(colorAttachmentStates.size()),
            .pAttachments = colorAttachmentStates.data()
        };

        // Define dynamic states to use
        constexpr std::array dynamicStates
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        // Set up dynamic states
        const VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        // Set up graphics pipeline creation info using all the modules created before
        const VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputStateCreateInfo,
            .pInputAssemblyState = &inputAssemblyStateCreateInfo,
            .pViewportState = &viewportStateCreateInfo,
            .pRasterizationState = &rasterizationStateCreateInfo,
            .pMultisampleState = &multisampleStateCreateInfo,
            .pDepthStencilState = vulkanRenderPass.HasDepthAttachment() ? &depthStencilStateCreateInfo : nullptr,
            .pColorBlendState = &blendingStateCreateInfo,
            .pDynamicState = &dynamicStateCreateInfo,
            .layout = device.GetPipelineLayout(createInfo.pushConstantSize),
            .renderPass = vulkanRenderPass.GetVulkanRenderPass(),
            .subpass = createInfo.subpassIndex,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        // Create pipeline
        const VkResult result = device.GetFunctionTable().vkCreateGraphicsPipelines(device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create graphics pipeline [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Set object name
        device.SetResourceName(pipeline, VK_OBJECT_TYPE_PIPELINE, GetName());
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
