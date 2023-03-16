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
    template<typename PC = NullType, typename UB = NullType, typename SB = NullType>
    class Pipeline
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline PC& GetPushConstantData()
        {
            ASSERT_WARNING_IF(!HasPushConstants(), "Push constants have not been created for this compute pipeline");
            return *pushConstantData;
        }
        [[nodiscard]] inline UB& GetUniformBufferData()
        {
            ASSERT_WARNING_IF(!HasUniformBuffers(), "Uniform buffers have not been created for this compute pipeline");
            return *uniformBufferData;
        }
        [[nodiscard]] inline SB& GetStorageBufferData()
        {
            ASSERT_WARNING_IF(!HasStorageBuffers(), "Storage buffers have not been created for this compute pipeline");
            return *storageBufferData;
        }

        [[nodiscard]] inline UniquePtr<Buffer>& GetUniformBuffer(const uint index)
        {
            ASSERT_WARNING_IF(!HasUniformBuffers(), "Uniform buffers have not been created for this graphics pipeline");
            return *uniformBuffers[index].buffer;
        }
        [[nodiscard]] inline UniquePtr<Buffer>& GetStorageBuffer(const uint index)
        {
            ASSERT_WARNING_IF(!HasStorageBuffers(), "Storage buffers have not been created for this graphics pipeline");
            return *storageBuffers[index].buffer;
        }

        [[nodiscard]] inline UniquePtr<DescriptorSetLayout>& GetDescriptorSetLayout()
        {
            return *descriptorSetLayout;
        }

        [[nodiscard]] inline UniquePtr<DescriptorSet>& GetDescriptorSet(const uint forFrame = 0)
        {
            return descriptorSets[forFrame];
        }
        [[nodiscard]] inline std::vector<UniquePtr<DescriptorSet>>& GetDescriptorSets()
        {
            return descriptorSets;
        }

        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return vkPipeline; }
        [[nodiscard]] inline VkPipelineLayout GetVulkanPipelineLayout() const { return vkPipelineLayout; }

        /* --- SETTER METHODS --- */
        inline void Recreate()
        {
            CreatePipeline();
        }
        inline void Bind(const UniquePtr<CommandBuffer> &givenCommandBuffer)
        {
            vkCmdBindPipeline(givenCommandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipeline);

            if (!usesExternalUniformBuffers)
            {
                for (auto &bufferData : uniformBuffers)
                {
                    bufferData.updatedForFrame = false;
                }
            }

            if (!usesExternalStorageBuffers)
            {
                for (auto &bufferData : storageBuffers)
                {
                    bufferData.updatedForFrame = false;
                }
            }

            if (bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS) currentFrame = (currentFrame + 1) % maxConcurrentFrames;
        };
        inline void BindDescriptorSets(const UniquePtr<CommandBuffer> &givenCommandBuffer, UniquePtr<DescriptorSet> &externalDescriptorSet, const bool forceUpdateUniformBuffers = false, const bool forceUpdateStorageBuffers = false)
        {
            if (HasUniformBuffers() && (!uniformBuffers[currentFrame].updatedForFrame || forceUpdateUniformBuffers))
            {
                uniformBuffers[currentFrame].updatedForFrame = true;
                uniformBuffers[currentFrame].buffer->get()->CopyFromPointer(uniformBufferData);
            }

            if (HasStorageBuffers() && (!storageBuffers[currentFrame].updatedForFrame || forceUpdateStorageBuffers))
            {
                storageBuffers[currentFrame].updatedForFrame = true;
                storageBuffers[currentFrame].buffer->get()->CopyFromPointer(storageBufferData);
            }

            std::vector<VkDescriptorSet> finalDescriptorSetGroup { externalDescriptorSet->GetVulkanDescriptorSet() };

            if (!descriptorSets.empty()) finalDescriptorSetGroup.insert(finalDescriptorSetGroup.begin(), descriptorSets[currentFrame]->GetVulkanDescriptorSet());
            vkCmdBindDescriptorSets(givenCommandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipelineLayout, 0, finalDescriptorSetGroup.size(), finalDescriptorSetGroup.data(), 0, nullptr);
        };
        inline void BindDescriptorSets(const UniquePtr<CommandBuffer> &givenCommandBuffer, const bool forceUpdateUniformBuffers = false, const bool forceUpdateStorageBuffers = false)
        {
            if (HasUniformBuffers() && (!uniformBuffers[currentFrame].updatedForFrame || forceUpdateUniformBuffers))
            {
                uniformBuffers[currentFrame].updatedForFrame = true;
                uniformBuffers[currentFrame].buffer->get()->CopyFromPointer(uniformBufferData);
            }

            if (HasStorageBuffers() && (!storageBuffers[currentFrame].updatedForFrame || forceUpdateStorageBuffers))
            {
                storageBuffers[currentFrame].updatedForFrame = true;
                storageBuffers[currentFrame].buffer->get()->CopyFromPointer(storageBufferData);
            }

            VkDescriptorSet descriptorSetPtr;
            descriptorSetPtr = descriptorSets[currentFrame]->GetVulkanDescriptorSet();

            vkCmdBindDescriptorSets(givenCommandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipelineLayout, 0, 1, &descriptorSetPtr, 0, nullptr);
        };
        inline void PushConstants(const UniquePtr<CommandBuffer> &givenCommandBuffer)
        {
            ASSERT_ERROR_IF(pushConstantRange == nullptr, "Trying to push constants in pipeline that was created without support for them in mind");

            vkCmdPushConstants(
                    givenCommandBuffer->GetVulkanCommandBuffer(), vkPipelineLayout,
                    pushConstantRange->stageFlags, pushConstantRange->offset,
                    pushConstantRange->size, pushConstantData
            );
        };
        inline void OverloadShader(SharedPtr<Shader> newShader)
        {
            VK::GetDevice()->WaitUntilIdle();

            for (uint i = (*shaders).size(); i--;)
            {
                if ((*shaders)[i]->GetShaderType() == newShader->GetShaderType())
                {
                    (*shaders)[i] = newShader;
                    CreatePipeline();

                    return;
                }
            }

            ASSERT_ERROR_FORMATTED("Shader of type [{0}] could not be overloaded because it is not present in the pipeline", VK_TO_STRING(newShader->GetShaderType(), ShaderStageFlagBits));
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint GetMaxConcurrentFrames() const { return maxConcurrentFrames; }

        /* --- DESTRUCTOR --- */
        inline virtual void Destroy()
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

                File::WriteBinaryDataToFile(File::INTERNAL_TEMP_FOLDER_PATH + "PipelineCache/PipelineCache_" + std::to_string(GetCacheHash()), cacheData, true);
            }

            if (!usesExternalPushConstants)
            {
                delete pushConstantData;
                delete pushConstantRange;
            }

            if (uniformBufferData != nullptr)
            {
                if (!usesExternalUniformBuffers)
                {
                    delete uniformBufferData;

                    for (const auto &uniformBuffer : uniformBuffers)
                    {
                        uniformBuffer.buffer->get()->Destroy();
                        delete uniformBuffer.buffer;
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
                        storageBuffer.buffer->get()->Destroy();
                        delete storageBuffer.buffer;
                    }
                }
            }

            if (!usesExternalDescriptorSetLayout && descriptorSetLayout != nullptr)
            {
                (*descriptorSetLayout)->Destroy();
                delete descriptorSetLayout;
            }

            vkDestroyPipelineCache(VK::GetLogicalDevice(), pipelineCache, nullptr);
            vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
            vkDestroyPipelineLayout(VK::GetLogicalDevice(), vkPipelineLayout, nullptr);
        };
        DELETE_COPY(Pipeline);

    private:
        struct BufferData
        {
            UniquePtr<Buffer> *buffer;
            bool updatedForFrame = false;
        };

    protected:
        inline Pipeline<PC, UB, SB>() = default;

        inline virtual void SaveUniversalInfo() { };
        inline void CreateDescriptors(UniquePtr<DescriptorSetLayout> *providedLayout)
        {
            if (providedLayout != nullptr)
            {
                descriptorSetLayout = providedLayout;

                usesExternalDescriptorSetLayout = true;
                return;
            }

            DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};

            for (const auto &shader : *shaders)
            {
                for (const auto &binding : *shader->precompiledData->reflectionData.descriptorBindings)
                {
                    auto &reference = descriptorSetLayoutCreateInfo.bindings[binding.binding];
                    reference.shaderStages |= shader->GetShaderType();
                    reference.descriptorType = static_cast<DescriptorType>(binding.fieldType);
                    reference.arraySize = binding.arraySize;
                }

                delete shader->precompiledData->reflectionData.descriptorBindings;
            }

            if (descriptorSetLayoutCreateInfo.bindings.empty())
            {
                descriptorSetLayout = nullptr;
                return;
            }

            descriptorSetLayout = new UniquePtr<DescriptorSetLayout>(DescriptorSetLayout::Create(descriptorSetLayoutCreateInfo));
        };
        inline void CreatePipelineLayout()
        {
            // Set pipeline layout creation info
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            // Create push constant
            if constexpr (!std::is_same_v<PC, NullType>)
            {
                if (pushConstantData == nullptr)
                {
                    pushConstantData = new PC();

                    pushConstantRange = new VkPushConstantRange();
                    pushConstantRange->size = sizeof(PC);
                    pushConstantRange->offset = 0;
                    pushConstantRange->stageFlags = (VkShaderStageFlags) pushConstantShaderStages;

                    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
                    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRange;
                }
            }

            // Create uniform buffer
            if constexpr (!std::is_same_v<UB, NullType>)
            {
                // Check if user has provided external storage buffers
                if (!HasUniformBuffers())
                {
                    uniformBufferData = new UB();

                    uniformBuffers.resize(maxConcurrentFrames);
                    for (uint i = maxConcurrentFrames; i--;)
                    {
                        uniformBuffers[i] = { new UniquePtr<Buffer>(new Buffer({
                            .memorySize = sizeof(UB),
                            .bufferUsage = BufferUsage::UNIFORM
                        })) };
                    }
                }
            }

            // Create storage buffer
            if constexpr (!std::is_same_v<SB, NullType>)
            {
                // Check if user has provided external storage buffers
                if (!HasStorageBuffers())
                {
                    storageBufferData = new SB();

                    storageBuffers.resize(maxConcurrentFrames);
                    for (uint i = maxConcurrentFrames; i--;)
                    {
                        storageBuffers[i] = { new UniquePtr<Buffer>(new Buffer({
                            .memorySize = sizeof(SB),
                            .bufferUsage = BufferUsage::STORAGE
                        })) };
                    }
                }
            }

            descriptorSets.resize((uint) (HasUniformBuffers() || HasStorageBuffers()) * maxConcurrentFrames);
            if (!descriptorSets.empty())
            {
                for (uint i = 0; i < maxConcurrentFrames; i++)
                {
                    descriptorSets[i] = DescriptorSet::Create(*descriptorSetLayout);
                    if (HasUniformBuffers()) descriptorSets[i]->WriteBuffer(UNIFORM_BUFFER_BINDING, *uniformBuffers[i].buffer);
                    if (HasStorageBuffers()) descriptorSets[i]->WriteBuffer(STORAGE_BUFFER_BINDING, *storageBuffers[i].buffer);
                    descriptorSets[i]->Allocate();
                }
            }

            // Add descriptor layout
            VkDescriptorSetLayout* descriptorSetLayoutsPtr = new VkDescriptorSetLayout[(uint) (descriptorSetLayout != nullptr) * 2];
            if (descriptorSetLayout != nullptr)
            {
                descriptorSetLayoutsPtr[0] = (*descriptorSetLayout)->GetVulkanDescriptorSetLayout();
                descriptorSetLayoutsPtr[1] = (*descriptorSetLayout)->GetVulkanDescriptorSetLayout();

                pipelineLayoutCreateInfo.setLayoutCount = 2;
                pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayoutsPtr;
            }

            // Create the pipeline layout
            VK_ASSERT(
                vkCreatePipelineLayout(VK::GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout),
                "Failed to create pipeline layout"
            );

            delete[] descriptorSetLayoutsPtr;
        };
        inline virtual void CreatePipeline() { };

        VkPipeline vkPipeline;
        VkPipelineLayout vkPipelineLayout;

        PC* pushConstantData = nullptr;
        VkPushConstantRange* pushConstantRange = nullptr;

        UB* uniformBufferData = nullptr;
        std::vector<BufferData> uniformBuffers;

        SB* storageBufferData = nullptr;
        std::vector<BufferData> storageBuffers;

        std::vector<SharedPtr<Shader>>* shaders = nullptr;
        std::vector<UniquePtr<DescriptorSet>> descriptorSets;

        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        UniquePtr<DescriptorSetLayout> *descriptorSetLayout = nullptr;

        ShaderType pushConstantShaderStages;

        bool alreadyCreated = false;
        bool usesExternalPushConstants = false;
        bool usesExternalUniformBuffers = false;
        bool usesExternalStorageBuffers = false;
        bool usesExternalDescriptorSetLayout = false;

        uint currentFrame = 0;
        uint maxConcurrentFrames = 1;

        VkPipelineBindPoint bindPoint;

        inline bool HasPushConstants() const { return pushConstantRange != nullptr; }
        inline bool HasUniformBuffers() const { return !uniformBuffers.empty() && uniformBuffers[0].buffer != nullptr; }
        inline bool HasStorageBuffers() const { return !storageBuffers.empty() && storageBuffers[0].buffer != nullptr; }
        inline uSize GetCacheHash()
        {
            String stringToCache;
            for (const auto &shader : *shaders)
            {
                stringToCache += shader->GetFilePath();
            }

            return GET_HASH(stringToCache);
        }

        template<typename OtherPC, typename OtherUB, typename OtherSB>
        inline void CopyDataInPipelines(Pipeline<OtherPC, OtherUB, OtherSB> *copyFrom, const PipelineCopyOp copyOp)
        {
            if constexpr(!std::is_same_v<PC, NullType> && std::is_same_v<PC, OtherPC>)
            {
                if (IS_FLAG_PRESENT(copyOp, PipelineCopyOp::PUSH_CONSTANTS))
                {
                    usesExternalPushConstants = true;
                    pushConstantData = copyFrom->pushConstantData;
                    pushConstantRange = copyFrom->pushConstantRange;
                }
            }

            if constexpr(!std::is_same_v<UB, NullType> && std::is_same_v<OtherUB, OtherUB>)
            {
                if (IS_FLAG_PRESENT(copyOp, PipelineCopyOp::UNIFORM_BUFFERS))
                {
                    usesExternalUniformBuffers = true;
                    uniformBufferData = &copyFrom->GetUniformBufferData();

                    uniformBuffers.resize(maxConcurrentFrames);
                    for (uint i = maxConcurrentFrames; i--;) uniformBuffers[i].buffer = &copyFrom->GetUniformBuffer(i);
                }
            }

            if constexpr(!std::is_same_v<SB, NullType> && std::is_same_v<SB, OtherSB>)
            {
                if (IS_FLAG_PRESENT(copyOp, PipelineCopyOp::STORAGE_BUFFERS))
                {
                    usesExternalStorageBuffers = true;
                    storageBufferData = &copyFrom->GetStorageBufferData();

                    storageBuffers.resize(maxConcurrentFrames);
                    for (uint i = maxConcurrentFrames; i--;) storageBuffers[i].buffer = &copyFrom->GetStorageBuffer(i);
                }
            }

        }
    };

    // ==================== GRAPHICS PIPELINE ==================== \\

    struct GraphicsPipelineDynamicRenderingInfo
    {
        std::vector<ImageReference> attachments;
    };

    struct GraphicsPipelineRenderPassInfo
    {
        const UniquePtr<RenderPass> *renderPass;
        uint subpasss = 0;
        uint colorAttachmentCount = 1;
        bool createDepthBuffer = false;
    };

    struct GraphicsPipelineShaderData
    {
        std::vector<VertexAttributeType> vertexAttributes;
        UniquePtr<DescriptorSetLayout> *descriptorSetLayout = nullptr;
    };

    struct GraphicsPipelineCreateInfo
    {
        uint maxConcurrentFrames;
        std::vector<SharedPtr<Shader>> shaders;
        Optional<GraphicsPipelineShaderData> shaderData;

        Optional<GraphicsPipelineDynamicRenderingInfo> dynamicRenderingInfo;
        // OR
        Optional<GraphicsPipelineRenderPassInfo> renderPassInfo;

        ShaderType pushConstantShaderStages = ShaderType::VERTEX | ShaderType::FRAGMENT;
        Sampling sampling = Sampling::MSAAx1;
        FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
        CullMode cullMode = CullMode::FRONT;
        ShadingType shadingType = ShadingType::FILL;
    };

    template<typename PC = NullType, typename UB = NullType, typename SB = NullType>
    class GraphicsPipeline : public Pipeline<PC, UB, SB>
    {
    public:

        /* --- CONSTRUCTORS --- */
        inline GraphicsPipeline() = default;

        inline GraphicsPipeline(const GraphicsPipelineCreateInfo &givenCreateInfo)
            : createInfo(std::move(givenCreateInfo))
        {
            this->SaveUniversalInfo();
            this->CreateDescriptors(createInfo.shaderData.has_value() ? createInfo.shaderData.value().descriptorSetLayout : nullptr);
            this->CreatePipelineLayout();
            this->CreatePipeline();
        }

        inline static UniquePtr<GraphicsPipeline> Create(GraphicsPipelineCreateInfo createInfo)
        {
            return std::make_unique<GraphicsPipeline>(createInfo);
        };

        template<typename OtherPC, typename OtherUB, typename OtherSB>
        inline static UniquePtr<GraphicsPipeline> CreateFromAnotherPipeline(GraphicsPipelineCreateInfo createInfo, const UniquePtr<GraphicsPipeline<OtherPC, OtherUB, OtherSB>> &copyFrom, const PipelineCopyOp copyOp)
        {
            UniquePtr<GraphicsPipeline<PC, UB, SB>> pipeline = std::make_unique<GraphicsPipeline<PC, UB, SB>>();

            pipeline->createInfo = std::move(createInfo);
            pipeline->SaveUniversalInfo();

            pipeline->CopyDataInPipelines(copyFrom.get(), copyOp);

            pipeline->CreateDescriptors(createInfo.shaderData.has_value() ? createInfo.shaderData.value().descriptorSetLayout : nullptr);
            pipeline->CreatePipelineLayout();
            pipeline->CreatePipeline();

            return pipeline;
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline GraphicsPipelineCreateInfo& GetCreateInfo() { return createInfo; }

        /* --- SETTER METHODS --- */
        inline void SetShaderDefinition(const ShaderType shaderType, const ShaderDefinition definition)
        {
            VK::GetDevice()->WaitUntilIdle();

            for (uint i = createInfo.shaders.size(); i--;)
            {
                if (createInfo.shaders[i]->GetShaderType() == shaderType)
                {
                    if (createInfo.shaders[i]->SetDefinition(definition))
                    {
                        CreatePipeline();
                    }

                    return;
                }
            }

            ASSERT_ERROR_FORMATTED("Shader of type [{0}] could not be overloaded because it is not present in the pipeline", VK_TO_STRING(shaderType, ShaderStageFlagBits));
        }

        /* --- DESTRUCTOR --- */
        DELETE_COPY(GraphicsPipeline);
    private:
        GraphicsPipelineCreateInfo createInfo;
        inline void SaveUniversalInfo() override
        {
            this->shaders = &createInfo.shaders;
            this->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            this->maxConcurrentFrames = createInfo.maxConcurrentFrames;
            this->pushConstantShaderStages = createInfo.pushConstantShaderStages;
        }
        inline void CreatePipeline() override
        {
            if (this->alreadyCreated)
            {
                vkDestroyPipeline(VK::GetLogicalDevice(), this->vkPipeline, nullptr);
            }

            VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[createInfo.shaders.size()];

            int vertexShaderIndex = -1;
            for (uint i = createInfo.shaders.size(); i--;)
            {
                shaderStages[i] = createInfo.shaders[i]->GetVkShaderStageInfo();
                if (createInfo.shaders[i]->GetShaderType() == ShaderType::VERTEX) vertexShaderIndex = i;
            }

            // Define the attributes to be sent to the shader
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

            uint lastOffset = 0;
            if (vertexShaderIndex != -1)
            {
                if (createInfo.shaderData.has_value())
                {
                    attributeDescriptions.resize(createInfo.shaderData.value().vertexAttributes.size());

                    for (uint i = 0; i < attributeDescriptions.size(); i++)
                    {
                        attributeDescriptions[i].binding = 0;
                        attributeDescriptions[i].location = i;
                        attributeDescriptions[i].format = static_cast<VkFormat>(createInfo.shaderData.value().vertexAttributes[i]);
                        attributeDescriptions[i].offset = lastOffset;

                        lastOffset += GetVertexAttributeTypeSize(createInfo.shaderData.value().vertexAttributes[i]);
                    }
                }
                else
                {
                    if (createInfo.shaders[vertexShaderIndex]->precompiledData->reflectionData.vertexAttributes != nullptr)
                    {
                        auto &attributes = *createInfo.shaders[vertexShaderIndex]->precompiledData->reflectionData.vertexAttributes;
                        attributeDescriptions.resize(attributes.size());

                        for (uint i = 0; i < attributeDescriptions.size(); i++)
                        {
                            attributeDescriptions[i].binding = 0;
                            attributeDescriptions[i].location = attributes[i].location;
                            attributeDescriptions[i].format = static_cast<VkFormat>(attributes[i].vertexAttributeType);
                            attributeDescriptions[i].offset = lastOffset;

                            lastOffset += GetVertexAttributeTypeSize(attributes[i].vertexAttributeType);
                        }

                        delete createInfo.shaders[vertexShaderIndex]->precompiledData->reflectionData.vertexAttributes;
                    }
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

            bool hasDepthAttachment = false;

            ASSERT_ERROR_IF(!createInfo.renderPassInfo.has_value() && !createInfo.dynamicRenderingInfo.has_value(), "When creating graphics pipelines either [renderPassInfo] or [dynamicRenderingInfo] must be set");

            bool usesRenderPasses = createInfo.renderPassInfo.has_value();

            if (!usesRenderPasses)
            {
                // Check if depth attachment is set
                for (const auto attachment : createInfo.dynamicRenderingInfo.value().attachments)
                {
                    if (IS_FLAG_PRESENT(attachment.image->GetUsage(), ImageUsage::DEPTH_STENCIL_ATTACHMENT))
                    {
                        hasDepthAttachment = true;
                        break;
                    }
                }
            }

            // Calculate final color attachment count
            uint colorAttachmentCount = usesRenderPasses ? createInfo.renderPassInfo.value().colorAttachmentCount : (createInfo.dynamicRenderingInfo.value().attachments.size() - (uint) hasDepthAttachment);
            std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates;
            if (!usesRenderPasses)
            {
                colorAttachmentStates = std::vector<VkPipelineColorBlendAttachmentState>(colorAttachmentCount, blendingAttachmentState);
            }
            else
            {
                colorAttachmentStates = std::vector<VkPipelineColorBlendAttachmentState>(createInfo.renderPassInfo.value().colorAttachmentCount, blendingAttachmentState);
            }

            // Set up bindings
            VkPipelineColorBlendStateCreateInfo blendingStateCreateInfo{};
            blendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            blendingStateCreateInfo.logicOpEnable = VK_FALSE;
            blendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
            blendingStateCreateInfo.attachmentCount = colorAttachmentStates.size();
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
            VkPipelineRenderingCreateInfoKHR graphicsPipelineCreateInfoKHR{};
            graphicsPipelineCreateInfoKHR.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
            graphicsPipelineCreateInfoKHR.colorAttachmentCount = colorAttachmentCount;

            VkFormat* colorAttachmentFormats;
            if (!usesRenderPasses)
            {
                // Set up attachments
                VkFormat depthAttachmentFormat = VK_FORMAT_UNDEFINED;
                colorAttachmentFormats = new VkFormat[colorAttachmentCount];

                uint i = 0;
                for (const auto &attachment: createInfo.dynamicRenderingInfo.value().attachments)
                {
                    // Check wether attachment is color
                    if (!IS_FLAG_PRESENT(attachment.image->GetUsage(), ImageUsage::DEPTH_STENCIL_ATTACHMENT))
                    {
                        colorAttachmentFormats[i] = (VkFormat) attachment.image->GetFormat();
                        i++;
                    } else
                    {
                        depthAttachmentFormat = (VkFormat) attachment.image->GetFormat();
                    }
                }

                // Assign attachment info to pipeline
                graphicsPipelineCreateInfoKHR.pColorAttachmentFormats = colorAttachmentFormats;
                graphicsPipelineCreateInfoKHR.depthAttachmentFormat = depthAttachmentFormat;
                graphicsPipelineCreateInfoKHR.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
            }

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
            graphicsPipelineCreateInfo.layout = this->vkPipelineLayout;
            graphicsPipelineCreateInfo.subpass = 0;
            graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
            graphicsPipelineCreateInfo.basePipelineIndex = -1;

            if (!usesRenderPasses)
            {
                graphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
                graphicsPipelineCreateInfo.pNext = &graphicsPipelineCreateInfoKHR;
            }
            else
            {
                graphicsPipelineCreateInfo.renderPass = createInfo.renderPassInfo.value().renderPass->get()->GetVulkanRenderPass();
            }

            VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
            if (hasDepthAttachment)
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

            if (this->pipelineCache == VK_NULL_HANDLE)
            {
                uSize cacheHash = this->GetCacheHash();
                String cachePath = File::INTERNAL_TEMP_FOLDER_PATH + "PipelineCache/PipelineCache_" + std::to_string(cacheHash);

                if (File::FileExists(cachePath))
                {
                    std::vector<uint8> cacheData = File::ReadBinaryFile(cachePath);

                    VkPipelineCacheCreateInfo cacheCreateInfo{};
                    cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
                    cacheCreateInfo.initialDataSize = cacheData.size();
                    cacheCreateInfo.pInitialData = cacheData.data();

                    VK_ASSERT(vkCreatePipelineCache(VK::GetLogicalDevice(), &cacheCreateInfo, nullptr, &this->pipelineCache), "Could not create cache for pipeline");
                }
            }

            // Create the graphics pipeline
            VK_ASSERT(
                vkCreateGraphicsPipelines(VK::GetLogicalDevice(), this->pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &this->vkPipeline),
                "Failed to create graphics pipeline"
            );

            this->alreadyCreated = true;

            delete[] shaderStages;
            if (!usesRenderPasses) delete[] colorAttachmentFormats;
        };
    };

    // ==================== COMPUTE PIPELINE ==================== \\

    struct ComputePipelineShaderData
    {
        UniquePtr<DescriptorSetLayout> *descriptorSetLayout;
    };

    struct ComputePipelineCreateInfo
    {
        SharedPtr<Shader> shader;
        Optional<ComputePipelineShaderData> shaderData;
    };

    template<typename PC = NullType, typename UB = NullType, typename SB = NullType>
    class ComputePipeline : public Pipeline<PC, UB, SB> {
    public:
        /* --- CONSTRUCTORS --- */
        ComputePipeline() = default;

        inline ComputePipeline(const ComputePipelineCreateInfo &givenCreateInfo)
            : createInfo(std::move(givenCreateInfo))
        {
            this->SaveUniversalInfo();
            this->CreateDescriptors(createInfo.shaderData.has_value() ? createInfo.shaderData.value().descriptorSetLayout : nullptr);
            this->CreatePipelineLayout();
            this->CreatePipeline();
        }

        inline static UniquePtr<ComputePipeline> Create(ComputePipelineCreateInfo createInfo)
        {
            return std::make_unique<ComputePipeline>(createInfo);
        };

        template<typename OtherPC, typename OtherUB, typename OtherSB>
        inline static UniquePtr<ComputePipeline> CreateFromAnotherPipeline(ComputePipelineCreateInfo createInfo, const UniquePtr<GraphicsPipeline<OtherPC, OtherUB, OtherSB>> &copyFrom, const PipelineCopyOp copyOp)
        {
            UniquePtr<ComputePipeline<PC, UB, SB>> pipeline = std::make_unique<ComputePipeline<PC, UB, SB>>();

            pipeline->createInfo = std::move(createInfo);
            pipeline->SaveUniversalInfo();

            pipeline->CopyDataInPipelines(copyFrom.get(), copyOp);

            pipeline->CreateDescriptors(createInfo.shaderData.has_value() ? createInfo.shaderData.value().descriptorSetLayout : nullptr);
            pipeline->CreatePipelineLayout();
            pipeline->CreatePipeline();

            return pipeline;
        };

        template<typename OtherPC, typename OtherUB, typename OtherSB>
        inline static UniquePtr<ComputePipeline> CreateFromAnotherPipeline(ComputePipelineCreateInfo createInfo, const UniquePtr<ComputePipeline<OtherPC, OtherUB, OtherSB>> &copyFrom, const PipelineCopyOp copyOp)
        {
            UniquePtr<ComputePipeline<PC, UB, SB>> pipeline = std::make_unique<ComputePipeline<PC, UB, SB>>();

            pipeline->createInfo = std::move(createInfo);
            pipeline->SaveUniversalInfo();

            pipeline->CopyDataInPipelines(copyFrom.get(), copyOp);

            pipeline->CreateDescriptors(createInfo.shaderData.has_value() ? createInfo.shaderData.value().descriptorSetLayout : nullptr);
            pipeline->CreatePipelineLayout();
            pipeline->CreatePipeline();

            return pipeline;
        };

        /* --- SETTER METHODS --- */

        inline void SetShaderDefinition(const ShaderDefinition definition)
        {
            VK::GetDevice()->WaitUntilIdle();

            if (createInfo.shader->SetDefinition(definition))
            {
                CreatePipeline();
            }
        }

        /* --- DESTRUCTOR --- */
        inline void Destroy() override
        {
            Pipeline<PC, UB, SB>::Destroy();
            delete this->shaders;
        };
        DELETE_COPY(ComputePipeline);

    private:
        ComputePipelineCreateInfo createInfo;
        void SaveUniversalInfo() override
        {
            this->shaders = new std::vector<SharedPtr<Shader>> { createInfo.shader };
            this->bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
            this->maxConcurrentFrames = 1;
            this->pushConstantShaderStages = ShaderType::COMPUTE;
        }
        void CreatePipeline() override
        {
            if (this->alreadyCreated)
            {
                vkDestroyPipeline(VK::GetLogicalDevice(), this->vkPipeline, nullptr);
            }

            ASSERT_ERROR_FORMATTED_IF(createInfo.shader->GetShaderType() != ShaderType::COMPUTE, "Cannot bind a shader of type [{0}] to a compute pipeline! It must be of type [ShaderType::COMPUTE]", VK_TO_STRING(createInfo.shader->GetShaderType(), ShaderStageFlagBits));

            VkComputePipelineCreateInfo pipelineCreateInfo{};
            pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            pipelineCreateInfo.stage = createInfo.shader->GetVkShaderStageInfo();
            pipelineCreateInfo.layout = this->vkPipelineLayout;

            // Create the compute pipeline
            VK_ASSERT(
                vkCreateComputePipelines(VK::GetLogicalDevice(), this->pipelineCache, 1, &pipelineCreateInfo, nullptr, &this->vkPipeline),
                "Failed to create compute pipeline"
            );

            this->alreadyCreated = true;
        };
    };

}