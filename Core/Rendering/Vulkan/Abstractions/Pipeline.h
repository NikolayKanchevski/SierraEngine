//
// Created by Nikolay Kanchevski on 18.12.22.
//

#pragma once

#include "Shader.h"
#include "RenderPass.h"
#include "Descriptors.h"
#include "../VulkanTypes.h"
#include "../VK.h"
#include "../../RenderingSettings.h"
#include "../../../Engine/Classes/File.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct NullPushConstant { };
    struct NullUniformBuffer { };
    struct NullStorageBuffer { };

    struct PipelineCreateInfo
    {
        uint maxConcurrentFrames;
        const std::vector<VertexAttribute> vertexAttributes;

        std::vector<SharedPtr<Shader>> shaders;
        const UniquePtr<RenderPass> &renderPass;
        uint subpasss = 0;

        bool createDepthBuffer = true;
        SharedPtr<DescriptorSetLayout> descriptorSetLayout = nullptr;

        uint colorAttachmentCount = 1;
        ShaderType pushConstantShaderStages = ShaderType::VERTEX | ShaderType::FRAGMENT;

        Sampling sampling = Sampling::MSAAx1;
        FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
        CullMode cullMode = CullMode::FRONT;
        ShadingType shadingType = ShadingType::FILL;
    };

    typedef uint PipelineCopyOp;
    typedef enum _PipelineCopyOp
    {
        PIPELINE_COPY_OP_PUSH_CONSTANTS = 0,
        PIPELINE_COPY_OP_UNIFORM_BUFFER = 1,
        PIPELINE_COPY_OP_STORAGE_BUFFER = 2
    } _PipelineCopyOp;

    template<typename PC = NullPushConstant, typename UB = NullUniformBuffer, typename SB = NullStorageBuffer>
    class Pipeline {
    public:
        /* --- CONSTRUCTORS --- */
        inline Pipeline(const PipelineCreateInfo &givenCreateInfo)
                : createInfo(std::move(givenCreateInfo))
        {
            CreatePipelineLayout();
            CreatePipeline();
        }
        inline Pipeline(const PipelineCreateInfo &givenCreateInfo, PC *externalPushConstantData, UB *externalUniformData, UniquePtr<Buffer>* *externalUniformBuffers, SB *externalStorageData, UniquePtr<Buffer>* *externalStorageBuffers)
                : createInfo(std::move(givenCreateInfo))
        {
            if constexpr(!std::is_same<PC, NullPushConstant>::value)
            {
                if (externalPushConstantData != nullptr)
                {
                    usesExternalPushConstants = true;
                    pushConstantData = externalPushConstantData;
                }
            }

            if constexpr(!std::is_same<UB, NullUniformBuffer>::value)
            {
                if (externalUniformBuffers != nullptr)
                {
                    usesExternalUniformBuffers = true;
                    uniformBufferData = externalUniformData;

                    uniformBuffers.resize(createInfo.maxConcurrentFrames);
                    for (uint32_t i = createInfo.maxConcurrentFrames; i--;) uniformBuffers[i] = externalUniformBuffers[i];
                }
            }

            if constexpr(!std::is_same<SB, NullStorageBuffer>::value)
            {
                if (externalStorageBuffers != nullptr)
                {
                    usesExternalStorageBuffers = true;
                    storageBufferData = externalStorageData;

                    storageBuffers.resize(createInfo.maxConcurrentFrames);
                    for (uint32_t i = createInfo.maxConcurrentFrames; i--;) storageBuffers[i] = externalStorageBuffers[i];
                }
            }

            CreatePipelineLayout();
            CreatePipeline();
        }

        inline static UniquePtr<Pipeline> Create(PipelineCreateInfo createInfo)
        {
            return std::make_unique<Pipeline>(createInfo);
        };
        template<typename OtherPC, typename OtherUB, typename OtherSB>
        inline static UniquePtr<Pipeline> CreateFromAnotherPipeline(PipelineCreateInfo createInfo, const UniquePtr<Pipeline<OtherPC, OtherUB, OtherSB>> &otherPipeline, const PipelineCopyOp copyOperations)
        {
            PC* newPushConstantData = nullptr;
            if constexpr (std::is_same<PC, OtherPC>::value)
            {
                if (copyOperations & PIPELINE_COPY_OP_PUSH_CONSTANTS)
                {
                    newPushConstantData = &otherPipeline->GetPushConstantData();
                }
            }

            UB *newUniformData = nullptr;
            UniquePtr<Buffer> **newUniformBuffers = nullptr;
            if constexpr(std::is_same<UB, OtherUB>::value)
            {
                if (copyOperations & PIPELINE_COPY_OP_UNIFORM_BUFFER)
                {
                    newUniformData = &otherPipeline->GetUniformBufferData();
                    newUniformBuffers = otherPipeline->GetUniformBuffers().data();
                }
            }

            SB *newStorageData = nullptr;
            UniquePtr<Buffer> **newStorageBuffers = nullptr;
            if constexpr(std::is_same<SB, OtherSB>::value)
            {
                if (copyOperations & PIPELINE_COPY_OP_STORAGE_BUFFER)
                {
                    newStorageData = &otherPipeline->GetStorageBufferData();
                    newStorageBuffers = otherPipeline->GetStorageBuffers().data();
                }
            }

            return std::make_unique<Pipeline>(createInfo, newPushConstantData, newUniformData, newUniformBuffers, newStorageData, newStorageBuffers);
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline PC& GetPushConstantData()
        {
            ASSERT_WARNING_IF(!HasPushConstants(), "Push constants have not been created for this pipeline");
            return *pushConstantData;
        }
        [[nodiscard]] inline UB& GetUniformBufferData()
        {
            ASSERT_WARNING_IF(!HasUniformBuffers(), "Uniform buffers have not been created for this pipeline");
            return *uniformBufferData;
        }
        [[nodiscard]] inline SB& GetStorageBufferData()
        {
            ASSERT_WARNING_IF(!HasStorageBuffers(), "Storage buffers have not been created for this pipeline");
            return *storageBufferData;
        }

        [[nodiscard]] inline std::vector<UniquePtr<Buffer>*>& GetUniformBuffers()
        {
            return uniformBuffers;
        }
        [[nodiscard]] inline UniquePtr<Buffer>& GetUniformBuffer(const uint32_t index)
        {
            ASSERT_WARNING_IF(!HasUniformBuffers(), "Uniform buffers have not been created for this pipeline");
            return *uniformBuffers[index];
        }

        [[nodiscard]] inline std::vector<UniquePtr<Buffer>*>& GetStorageBuffers()
        {
            return storageBuffers;
        }
        [[nodiscard]] inline UniquePtr<Buffer>& GetStorageBuffer(const uint32_t index)
        {
            ASSERT_WARNING_IF(!HasStorageBuffers(), "Storage buffers have not been created for this pipeline");
            return *storageBuffers[index];
        }
        [[nodiscard]] inline std::vector<UniquePtr<DescriptorSet>>& GetDescriptorSets()
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
            localDescriptorsHaveBeenBoundForFrame = false;
            vkCmdBindPipeline(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vkPipeline);
        };
        inline void BindDescriptorSets(VkCommandBuffer givenCommandBuffer, std::vector<VkDescriptorSet> externalDescriptorSets, const uint currentFrame)
        {
            if (!localDescriptorsHaveBeenBoundForFrame)
            {
                if (HasUniformBuffers()) uniformBuffers[currentFrame]->get()->CopyFromPointer(uniformBufferData);
                if (HasStorageBuffers()) storageBuffers[currentFrame]->get()->CopyFromPointer(storageBufferData);

                localDescriptorsHaveBeenBoundForFrame = true;
            }

            if (!descriptorSets.empty()) externalDescriptorSets.insert(externalDescriptorSets.begin(), descriptorSets[currentFrame]->GetVulkanDescriptorSet());
            vkCmdBindDescriptorSets(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, externalDescriptorSets.size(), externalDescriptorSets.data(), 0, nullptr);
        };
        inline void BindDescriptorSets(VkCommandBuffer givenCommandBuffer, const uint currentFrame)
        {
            if (localDescriptorsHaveBeenBoundForFrame) return;

            uint count = descriptorSets.size() / createInfo.maxConcurrentFrames;
            VkDescriptorSet* localDescriptorSets = new VkDescriptorSet[count];

            if (HasUniformBuffers())
            {
                uniformBuffers[currentFrame]->get()->CopyFromPointer(uniformBufferData);
                localDescriptorSets[0] = descriptorSets[currentFrame * 1]->GetVulkanDescriptorSet();
            }

            if (HasStorageBuffers())
            {
                storageBuffers[currentFrame]->get()->CopyFromPointer(storageBufferData);
                localDescriptorSets[1] = descriptorSets[currentFrame * 2]->GetVulkanDescriptorSet();
            }

            localDescriptorsHaveBeenBoundForFrame = true;
            vkCmdBindDescriptorSets(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, count, localDescriptorSets, 0, nullptr);
        };
        inline void PushConstants(VkCommandBuffer givenCommandBuffer)
        {
            ASSERT_ERROR_IF(pushConstantRange == nullptr, "Trying to push constants in pipeline that was created without support for them in mind");

            vkCmdPushConstants(
                    givenCommandBuffer, vkPipelineLayout,
                    pushConstantRange->stageFlags, pushConstantRange->offset,
                    pushConstantRange->size, pushConstantData
            );
        };
        inline void OverloadShader(SharedPtr<Shader> newShader)
        {
            VK::GetDevice()->WaitUntilIdle();

            for (uint i = createInfo.shaders.size(); i--;)
            {
                if (createInfo.shaders[i]->GetShaderType() == newShader->GetShaderType())
                {
                    createInfo.shaders[i] = newShader;
                    CreatePipeline();

                    return;
                }
            }

            ASSERT_ERROR_FORMATTED("Shader of type [{0}] could not be overloaded because it is not present in the pipeline", static_cast<uint32_t>(newShader->GetShaderType()));
        };

        /* --- DESTRUCTOR --- */
        inline void Destroy()
        {
            // If no cache was read create new one and store it
            if (pipelineCache == VK_NULL_HANDLE)
            {
                VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
                pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

                vkCreatePipelineCache(VK::GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache);

                uSize cacheSize;
                vkGetPipelineCacheData(VK::GetLogicalDevice(), pipelineCache, &cacheSize, nullptr);

                std::vector<uint8> cacheData(cacheSize);
                vkGetPipelineCacheData(VK::GetLogicalDevice(), pipelineCache, &cacheSize, cacheData.data());

                File::WriteBinaryDataToFile(fmt::format(File::INTERNAL_TEMP_FOLDER_PATH + "PipelineCache/PipelineCache_{0}", GetCacheHash()), cacheData, true);
            }

            if (pushConstantRange != nullptr)
            {
                delete pushConstantRange;
                if (!usesExternalPushConstants)
                {
                    delete pushConstantData;
                }
            }

            if (uniformBufferData != nullptr)
            {
                if (!usesExternalUniformBuffers)
                {
                    delete uniformBufferData;

                    for (const auto &uniformBuffer : uniformBuffers)
                    {
                        uniformBuffer->get()->Destroy();
                        delete uniformBuffer;
                    }
                }
            }

            if (storageBufferData != nullptr)
            {
                if (!usesExternalStorageBuffers)
                {
                    delete storageBufferData;

                    for (const auto &storageBuffer : storageBuffers)
                    {
                        storageBuffer->get()->Destroy();
                        delete storageBuffer;
                    }
                }
            }

            vkDestroyPipelineCache(VK::GetLogicalDevice(), pipelineCache, nullptr);
            vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
            vkDestroyPipelineLayout(VK::GetLogicalDevice(), vkPipelineLayout, nullptr);
        };

        DELETE_COPY(Pipeline);

    private:
        VkPipeline vkPipeline;
        VkPipelineLayout vkPipelineLayout;

        PC* pushConstantData = nullptr;
        VkPushConstantRange* pushConstantRange = nullptr;

        UB* uniformBufferData = nullptr;
        std::vector<UniquePtr<Buffer>*> uniformBuffers;

        SB* storageBufferData = nullptr;
        std::vector<UniquePtr<Buffer>*> storageBuffers;

        std::vector<UniquePtr<DescriptorSet>> descriptorSets;

        VkPipelineCache pipelineCache = VK_NULL_HANDLE;

        bool alreadyCreated = false;
        bool usesExternalPushConstants = false;
        bool usesExternalUniformBuffers = false;
        bool usesExternalStorageBuffers = false;

        PipelineCreateInfo createInfo;

        bool localDescriptorsHaveBeenBoundForFrame = false;

        inline bool HasPushConstants() const { return pushConstantRange != nullptr; }
        inline bool HasUniformBuffers() const { return !uniformBuffers.empty() && uniformBuffers[0] != nullptr; }
        inline bool HasStorageBuffers() const { return !storageBuffers.empty() && storageBuffers[0] != nullptr; }

        inline uSize GetCacheHash()
        {
            String stringToCache;
            for (const auto &shader : createInfo.shaders)
            {
                stringToCache += shader->GetFilePath();
            }

            return std::hash<String>{}(stringToCache);
        }

        inline void CreatePipelineLayout()
        {
            // Set pipeline layout creation info
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            // Create push constant
            if constexpr (!std::is_same<PC, NullPushConstant>::value)
            {
                if (pushConstantData == nullptr)
                {
                    pushConstantData = new PC();
                }
                if (pushConstantRange == nullptr)
                {
                    pushConstantRange = new VkPushConstantRange();
                    pushConstantRange->size = sizeof(PC);
                    pushConstantRange->offset = 0;
                    pushConstantRange->stageFlags = (VkShaderStageFlags) createInfo.pushConstantShaderStages;

                    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
                    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRange;
                }
            }

            // Create uniform buffer
            if constexpr (!std::is_same<UB, NullUniformBuffer>::value)
            {
                // Check if user has provided external storage buffers
                if (!HasUniformBuffers())
                {
                    uniformBufferData = new UB();

                    uniformBuffers.reserve(createInfo.maxConcurrentFrames);
                    for (uint i = createInfo.maxConcurrentFrames; i--;)
                    {
                        uniformBuffers.push_back(new UniquePtr<Buffer>(new Buffer({
                            .memorySize = sizeof(UB),
                            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
                            .bufferUsage = BufferUsage::UNIFORM
                        })));
                    }
                }
            }

            // Create storage buffer
            if constexpr (!std::is_same<SB, NullStorageBuffer>::value)
            {
                // Check if user has provided external storage buffers
                if (!HasStorageBuffers())
                {
                    storageBufferData = new SB();

                    storageBuffers.resize(createInfo.maxConcurrentFrames);
                    for (uint i = createInfo.maxConcurrentFrames; i--;)
                    {
                        storageBuffers[i] = new UniquePtr<Buffer>(new Buffer({
                            .memorySize = sizeof(SB),
                            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
                            .bufferUsage = BufferUsage::STORAGE
                        }));
                    }
                }
            }

            descriptorSets.resize((HasUniformBuffers() || HasStorageBuffers()) * createInfo.maxConcurrentFrames);
            if (!descriptorSets.empty())
            {
                for (uint i = createInfo.maxConcurrentFrames; i--;)
                {
                    descriptorSets[i] = DescriptorSet::Build(createInfo.descriptorSetLayout);
                    if (HasUniformBuffers()) descriptorSets[i]->WriteBuffer(UNIFORM_BUFFER_BINDING, *uniformBuffers[i]);
                    if (HasStorageBuffers()) descriptorSets[i]->WriteBuffer(STORAGE_BUFFER_BINDING, *storageBuffers[i]);
                    descriptorSets[i]->Allocate();
                }
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
            for (uint i = createInfo.shaders.size(); i--;)
            {
                shaderStages[i] = createInfo.shaders[i]->GetVkShaderStageInfo();
            }

            // Define the attributes to be sent to the shader
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(createInfo.vertexAttributes.size());

            uint lastOffset = 0;
            for (uint i = 0; i < createInfo.vertexAttributes.size(); i++)
            {
                attributeDescriptions[i].binding = 0;
                attributeDescriptions[i].location = i;
                attributeDescriptions[i].offset = lastOffset;

                lastOffset += static_cast<uint32_t>(createInfo.vertexAttributes[i]);

                switch (createInfo.vertexAttributes[i])
                {
                    case VertexAttribute::FLOAT:
                        attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
                        break;
                    case VertexAttribute::VEC2:
                        attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
                        break;
                    case VertexAttribute::VEC3:
                        attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
                        break;
                    case VertexAttribute::VEC4:
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

            if (createInfo.sampling > Sampling::MSAAx1)
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

            std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates(createInfo.colorAttachmentCount, blendingAttachmentState);

            // Set up bindings
            VkPipelineColorBlendStateCreateInfo blendingStateCreateInfo{};
            blendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            blendingStateCreateInfo.logicOpEnable = VK_FALSE;
            blendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
            blendingStateCreateInfo.attachmentCount = createInfo.colorAttachmentCount;
            blendingStateCreateInfo.pAttachments = colorAttachmentStates.data();

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
            graphicsPipelineCreateInfo.subpass = createInfo.subpasss;
            graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
            graphicsPipelineCreateInfo.basePipelineIndex = -1;

            VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
            if (createInfo.createDepthBuffer)
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

            if (pipelineCache == VK_NULL_HANDLE)
            {
                uSize cacheHash = GetCacheHash();
                String cachePath = fmt::format(File::INTERNAL_TEMP_FOLDER_PATH + "PipelineCache/PipelineCache_{0}", cacheHash);

                if (File::FileExists(cachePath))
                {
                    std::vector<uint8> cacheData = File::ReadBinaryFile(cachePath);

                    VkPipelineCacheCreateInfo cacheCreateInfo{};
                    cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
                    cacheCreateInfo.initialDataSize = cacheData.size();
                    cacheCreateInfo.pInitialData = cacheData.data();

                    VK_ASSERT(vkCreatePipelineCache(VK::GetLogicalDevice(), &cacheCreateInfo, nullptr, &pipelineCache), "Could not create cache for pipeline");
                }
            }

            // Create the graphics pipeline
            VK_ASSERT(
                vkCreateGraphicsPipelines(VK::GetLogicalDevice(), pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipeline),
                "Failed to create graphics pipeline"
            );

            alreadyCreated = true;
        };

    };

}