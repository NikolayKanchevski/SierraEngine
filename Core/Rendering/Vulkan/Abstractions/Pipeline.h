//
// Created by Nikolay Kanchevski on 18.12.22.
//

#pragma once

#include <memory>
#include <vector>

#include "Shader.h"
#include "RenderPass.h"
#include "Descriptors.h"
#include "../VulkanTypes.h"
#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct NullPushConstant { };
    struct NullUniformBuffer { };
    struct NullStorageBuffer { };

    struct PipelineCreateInfo
    {
        uint32_t maxConcurrentFrames;
        const std::vector<VertexAttribute> vertexAttributes;

        std::vector<std::shared_ptr<Shader>> shaders;
        const std::unique_ptr<RenderPass> &renderPass;

        bool createDepthBuffer = true;
        std::shared_ptr<DescriptorSetLayout> descriptorSetLayout = nullptr;

        uint32_t pushConstantOffset = 0;
        ShaderType pushConstantShaderStages = VERTEX_SHADER | FRAGMENT_SHADER;

        Sampling sampling = MSAAx1;
        FrontFace frontFace = FRONT_FACE_COUNTER_CLOCKWISE;
        CullMode cullMode = CULL_FRONT;
        ShadingType shadingType = SHADE_FILL;
    };

    template<typename PC = NullPushConstant, typename UB = NullUniformBuffer, typename SB = NullStorageBuffer>
    class Pipeline {
    public:
        /* --- CONSTRUCTORS --- */
        inline Pipeline(const PipelineCreateInfo &givenCreateInfo)
            : createInfo(std::move(givenCreateInfo))
        {
            CreatePipelineLayout();
            CreatePipeline();
        };
        inline static std::unique_ptr<Pipeline> Create(PipelineCreateInfo createInfo)
        {
            return std::make_unique<Pipeline>(createInfo);
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline PC& GetPushConstantData()
        {
            ASSERT_WARNING_IF(!HasPushConstant(), "Push constants have not been created for this pipeline");
            return pushConstantData;
        }
        [[nodiscard]] inline UB& GetUniformBufferData()
        {
            ASSERT_WARNING_IF(!HasUniformBuffer(), "Uniform buffers have not been created for this pipeline");
            return uniformBufferData;
        }
        [[nodiscard]] inline SB& GetStorageBufferData()
        {
            ASSERT_WARNING_IF(!HasStorageBuffer(), "Storage buffers have not been created for this pipeline");
            return storageBufferData;
        }
        [[nodiscard]] inline std::vector<std::unique_ptr<DescriptorSet>>& GetDescriptorSets()
        {
            return descriptorSets;
        }

        [[nodiscard]] inline PipelineCreateInfo& GetCreateInfo() { return createInfo; }
        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return vkPipeline; }
        [[nodiscard]] inline VkPipelineLayout GetVulkanPipelineLayout() const { return vkPipelineLayout; }

        /* --- SETTER METHODS --- */
        inline void Recreate()
        {
            CreatePipeline();
        }
        inline void Bind(VkCommandBuffer givenCommandBuffer)
        {
            vkCmdBindPipeline(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vkPipeline);
        };
        inline void BindDescriptorSets(VkCommandBuffer givenCommandBuffer, std::vector<VkDescriptorSet> givenDescriptorSets, const uint32_t currentFrame)
        {
            if (HasUniformBuffer())
            {
                uniformBuffers[currentFrame]->CopyFromPointer(&uniformBufferData);

            }
            if (HasStorageBuffer()) storageBuffers[currentFrame]->CopyFromPointer(&storageBufferData);

            givenDescriptorSets.insert(givenDescriptorSets.begin(), descriptorSets[currentFrame]->GetVulkanDescriptorSet());

            vkCmdBindDescriptorSets(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, givenDescriptorSets.size(), givenDescriptorSets.data(), 0, nullptr);
        };
        inline void PushConstants(VkCommandBuffer givenCommandBuffer)
        {
            ASSERT_ERROR_IF(pushConstantRange == nullptr, "Trying to push constants in pipeline that was created without support for them in mind");

            vkCmdPushConstants(
                    givenCommandBuffer, vkPipelineLayout,
                    pushConstantRange->stageFlags, pushConstantRange->offset,
                    pushConstantRange->size, &pushConstantData
            );
        };
        inline void OverloadShader(std::shared_ptr<Shader> newShader)
        {
            VK::GetDevice()->WaitUntilIdle();

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
        };

        /* --- DESTRUCTOR --- */
        inline void Destroy()
        {
            if (pushConstantRange != nullptr)
            {
                delete pushConstantRange;
            }

            for (const auto &uniformBuffer : uniformBuffers)
            {
                uniformBuffer->Destroy();
            }

            for (const auto &storageBuffer : storageBuffers)
            {
                storageBuffer->Destroy();
            }

            vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
            vkDestroyPipelineLayout(VK::GetLogicalDevice(), vkPipelineLayout, nullptr);
        };

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;

    private:
        VkPipeline vkPipeline;
        VkPipelineLayout vkPipelineLayout;

        PC pushConstantData;
        VkPushConstantRange *pushConstantRange = nullptr;

        UB uniformBufferData;
        std::vector<std::unique_ptr<Buffer>> uniformBuffers;

        SB storageBufferData;
        std::vector<std::unique_ptr<Buffer>> storageBuffers;

        std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;

        bool alreadyCreated = false;
        PipelineCreateInfo createInfo;

        bool HasPushConstant() const { return pushConstantRange != nullptr; }
        bool HasUniformBuffer() const { return !uniformBuffers.empty(); }
        bool HasStorageBuffer() const { return !storageBuffers.empty(); }

        void CreatePipelineLayout()
        {
            pushConstantData = PC {};
            uniformBufferData = UB {};
            storageBufferData = SB {};

            // Set pipeline layout creation info
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            // Create push constant
            if constexpr (!std::is_same<PC, NullPushConstant>::value)
            {
                pushConstantData = PC{};

                pushConstantRange = new VkPushConstantRange();
                pushConstantRange->size = sizeof(PC);
                pushConstantRange->offset = 0;
                pushConstantRange->stageFlags = createInfo.pushConstantShaderStages;

                pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
                pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRange;
            }

            // Create uniform buffer
            if constexpr (!std::is_same<UB, NullUniformBuffer>::value)
            {
                uniformBuffers.reserve(createInfo.maxConcurrentFrames);
                for (uint32_t i = createInfo.maxConcurrentFrames; i--;)
                {
                    uniformBuffers.push_back(Buffer::Create({
                        .memorySize = sizeof(UB),
                        .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
                        .bufferUsage = UNIFORM_BUFFER
                    }));
                }
            }

            // Create storage buffer
            if constexpr (!std::is_same<SB, NullStorageBuffer>::value)
            {
                storageBuffers.reserve(createInfo.maxConcurrentFrames);
                for (uint32_t i = createInfo.maxConcurrentFrames; i--;)
                {
                    storageBuffers.push_back(Buffer::Create({
                        .memorySize = sizeof(SB),
                        .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
                        .bufferUsage = STORAGE_BUFFER
                    }));
                }
            }

            descriptorSets.resize((HasUniformBuffer() || HasStorageBuffer()) * createInfo.maxConcurrentFrames );
            for (uint32_t i = createInfo.maxConcurrentFrames; i--;)
            {
                descriptorSets[i] = DescriptorSet::Build(createInfo.descriptorSetLayout);
                if (HasUniformBuffer()) descriptorSets[i]->WriteBuffer(UNIFORM_BUFFER_BINDING, uniformBuffers[i]);
                if (HasStorageBuffer()) descriptorSets[i]->WriteBuffer(STORAGE_BUFFER_BINDING, storageBuffers[i]);
                descriptorSets[i]->Allocate();
            }

            // Add descriptor layout
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
            if (createInfo.descriptorSetLayout != nullptr)
            {
                descriptorSetLayouts = std::vector(createInfo.maxConcurrentFrames, createInfo.descriptorSetLayout->GetVulkanDescriptorSetLayout());

                pipelineLayoutCreateInfo.setLayoutCount = createInfo.maxConcurrentFrames;
                pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
            }

            // Create the pipeline layout
            VK_ASSERT(
                vkCreatePipelineLayout(VK::GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout),
                "Failed to create pipeline layout"
            );
        };
        void CreatePipeline()
        {
            if (alreadyCreated)
            {
                vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
            }

            VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[createInfo.shaders.size()];
            for (uint32_t i = createInfo.shaders.size(); i--;)
            {
                shaderStages[i] = createInfo.shaders[i]->GetVkShaderStageInfo();
            }

            // Define the attributes to be sent to the shader
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(createInfo.vertexAttributes.size());

            uint32_t lastOffset = 0;
            for (uint32_t i = 0; i < createInfo.vertexAttributes.size(); i++)
            {
                attributeDescriptions[i].binding = 0;
                attributeDescriptions[i].location = i;
                attributeDescriptions[i].offset = lastOffset;

                lastOffset += (uint32_t) createInfo.vertexAttributes[i];

                switch (createInfo.vertexAttributes[i])
                {
                    case VERTEX_ATTRIBUTE_FLOAT:
                        attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
                        break;
                    case VERTEX_ATTRIBUTE_VEC2:
                        attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
                        break;
                    case VERTEX_ATTRIBUTE_VEC3:
                        attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
                        break;
                    case VERTEX_ATTRIBUTE_VEC4:
                        attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                        break;
                }
            }

            // Set up binding description
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = lastOffset;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            // Set up how vertex data is sent
            VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
            vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            if (!createInfo.vertexAttributes.empty())
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
            rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
            rasterizationStateCreateInfo.polygonMode = (VkPolygonMode) createInfo.shadingType;
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

            VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
            if (createInfo.createDepthBuffer)
            {
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
                vkCreateGraphicsPipelines(VK::GetLogicalDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipeline),
                "Failed to create graphics pipeline"
            );

            alreadyCreated = true;
        };

    };

}
