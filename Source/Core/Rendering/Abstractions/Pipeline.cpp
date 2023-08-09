//
// Created by Nikolay Kanchevski on 18.05.23.
//

#include "Pipeline.h"

#include "../Bases/VK.h"
#include "../../../Engine/Classes/File.h"
#include "../../../Engine/Handlers/Project.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    typedef std::unordered_map<ShaderMemberKey, ShaderMember, ShaderMemberKey::KeyHash, ShaderMemberKey::KeyEquality> ShaderMemberTable;
    void MergeShaderMemberMaps(const ShaderMemberTable &sourceMap, ShaderMemberTable &targetMap, const std::function<void(ShaderMember &, const ShaderMember &)>& MergeLogic)
    {
        for (auto &[key, value] : sourceMap)
        {
            auto iterator = targetMap.find(key);
            if (iterator != targetMap.end())
            {
                // Key exists in targetMap, apply custom merge logic
                MergeLogic(iterator->second, value);
            }
            else
            {
                // Key doesn't exist in targetMap, insert it
                targetMap.insert({ key, value });
            }
        }
    }

    Pipeline::Pipeline(const PipelineCreateInfo &createInfo)
        : MAX_CONCURRENT_FRAMES(createInfo.maxConcurrentFrames), usesPrecompiledShaders(createInfo.shaders[0]->IsPrecompiled()), descriptorSetCount(createInfo.shaderInfo.has_value() ? createInfo.shaderInfo->descriptorSetCount : 1)
    {
        // Save given shaders in create info and load their members
        shaders.reserve(createInfo.shaders.size());
        for (const auto &shaderInCreateInfo : createInfo.shaders)
        {
            // Add shader from create info to local shader array
            auto &shader = shaders[shaderInCreateInfo->GetShaderType()];
            shader = std::move(shaderInCreateInfo);

            #if DEBUG
                // A check for wether unsuitable shaders have been passed
                if (shader->IsPrecompiled() != usesPrecompiledShaders)
                {
                    ASSERT_ERROR("Using a combination of compiled and precompiled shaders for a single pipeline is illegal");
                }
            #endif

            if (!usesPrecompiledShaders) continue;

            // Merge members
            MergeShaderMemberMaps(*shader->GetPrecompiledData()->reflectionData.members, shaderMembers, [this](ShaderMember &existingValue, const ShaderMember &newValue)
            {
                existingValue.shaderStages |= newValue.shaderStages;
                if (existingValue.memberType != ShaderMemberType::PUSH_CONSTANT)
                {
                    existingValue.descriptorData->bindings.insert(*newValue.descriptorData->bindings.begin());
                }
                else
                {
                    pushConstantMemory = existingValue.pushConstantData->data.get()->GetData();
                }

                delete(newValue.descriptorData);
            });

            delete(shader->GetPrecompiledData()->reflectionData.members);
        }

        // Create descriptor set layout and push constants
        if (usesPrecompiledShaders)
        {
            DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
            descriptorSetLayoutCreateInfo.flags = DescriptorSetLayoutFlag::PUSH_DESCRIPTOR_KHR;
            for (const auto &[bindingKey, bindingData] : shaderMembers)
            {
                if (bindingData.memberType == ShaderMemberType::PUSH_CONSTANT)
                {
                    // Setup push constant handling
                    pushConstantRange = new VkPushConstantRange();
                    pushConstantRange->offset = 0;
                    pushConstantRange->size = bindingData.pushConstantData->memorySize;
                    pushConstantRange->stageFlags = static_cast<VkShaderStageFlags>(bindingData.shaderStages);

                    // Allocate push constant data
                    bindingData.pushConstantData->data = std::make_unique<Engine::MemoryObject>(bindingData.pushConstantData->memorySize);
                    pushConstantMemory = bindingData.pushConstantData->data->GetData();
                    continue;
                }
                else if (bindingData.memberType == ShaderMemberType::SPECIALIZATION_CONSTANT)
                {
                    continue;
                }
                else if (bindingData.memberType == ShaderMemberType::BUFFER)
                {
                    // Resize buffers array
                    bindingData.bufferData->buffers.resize(MAX_CONCURRENT_FRAMES);
                }

                // Reference member data in descriptor set layout
                for (const auto &binding : bindingData.descriptorData->bindings)
                {
                    auto &reference = descriptorSetLayoutCreateInfo.bindings[binding];
                    reference.shaderStages = bindingData.shaderStages;
                    reference.descriptorType = bindingData.descriptorData->type;
                    reference.arraySize = bindingData.descriptorData->arraySize;
                }
            }

            // Create descriptor set layout
            if (!descriptorSetLayoutCreateInfo.bindings.empty()) descriptorSetLayout = new UniquePtr<DescriptorSetLayout>(DescriptorSetLayout::Create(descriptorSetLayoutCreateInfo));
        }
        else
        {
            // Create push constant range
            if (createInfo.shaderInfo.has_value() && createInfo.shaderInfo->pushConstantData.has_value())
            {
                pushConstantRange = new VkPushConstantRange();
                pushConstantRange->offset = 0;
                pushConstantRange->size = createInfo.shaderInfo->pushConstantData->size;
                pushConstantRange->stageFlags = static_cast<VkShaderStageFlags>(createInfo.shaderInfo->pushConstantData->shaderStages);
            }

            // Copy passed descriptor set layout
            if (createInfo.shaderInfo.has_value() && createInfo.shaderInfo->descriptorSetLayout != nullptr)
            {
                descriptorSetLayout = createInfo.shaderInfo->descriptorSetLayout;
            }
        }

        if (descriptorSetLayout != nullptr)
        {
            ASSERT_ERROR_IF(!VK::GetDevice()->IsExtensionLoaded(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME), "Cannot create use push descriptors in pipeline, unless extension [{0}] is supported and loaded", VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);

            // Create descriptor sets
            descriptorSets.resize(MAX_CONCURRENT_FRAMES);
            for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
            {
                descriptorSets[i] = PushDescriptorSet::Create({ .descriptorSetIndex = 0 });
            }
        }

        // Load cache if present
        String cachePath = GetPipelineCacheFilePath();
        if (Engine::File::FileExists(cachePath))
        {
            std::vector<uint8> cacheData = Engine::File::ReadBinaryFile(cachePath);

            VkPipelineCacheCreateInfo cacheCreateInfo{};
            cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            cacheCreateInfo.initialDataSize = cacheData.size();
            cacheCreateInfo.pInitialData = cacheData.data();

            VK_ASSERT(vkCreatePipelineCache(VK::GetLogicalDevice(), &cacheCreateInfo, nullptr, &pipelineCache), "Could not create cache for pipeline");
        }

        CreatePipelineLayout();
    }

    void Pipeline::CreatePipelineLayout()
    {
        // Set pipeline layout creation info
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        if (pushConstantRange != nullptr)
        {
            pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
            pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRange;
        }

        std::vector<VkDescriptorSetLayout> descriptorSetLayoutsPtr;
        if (descriptorSetLayout != nullptr)
        {
            descriptorSetLayoutsPtr = std::vector<VkDescriptorSetLayout>(descriptorSetCount, descriptorSetLayout->get()->GetVulkanDescriptorSetLayout());
            pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayoutsPtr.size();
            pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayoutsPtr.data();
        }

        // Create the pipeline layout
        VK_ASSERT(
            vkCreatePipelineLayout(VK::GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout),
            "Failed to create pipeline layout"
        );
    }

    /* --- POLLING METHODS --- */

    void Pipeline::Begin(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // Set bound status
        bound = true;

        // Bind pipeline
        vkCmdBindPipeline(commandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipeline);

        // Send data to shaders
        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::BindDescriptorSet(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<DescriptorSet> &givenDescriptorSet, const uint32 offset) const
    {
        // Get Vulkan descriptor set
        VkDescriptorSet vkDescriptorSet = givenDescriptorSet->GetVulkanDescriptorSet();

        // Bind set
        vkCmdBindDescriptorSets(commandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipelineLayout, offset, 1, &vkDescriptorSet, 0, 0);
    }

    void Pipeline::BindDescriptorSets(const UniquePtr<CommandBuffer> &commandBuffer, const std::vector<ReferenceWrapper<UniquePtr<DescriptorSet>>> &givenDescriptorSets, const uint32 offset) const
    {
        // Create a temporary array of the set type Vulkan is expecting
        VkDescriptorSet* vkDescriptorSetsPtr = new VkDescriptorSet[givenDescriptorSets.size()];
        for (uint32 i = givenDescriptorSets.size(); i--;)
        {
            vkDescriptorSetsPtr[i] = givenDescriptorSets[i].get()->GetVulkanDescriptorSet();
        }

        // Bind temporary descriptor array
        vkCmdBindDescriptorSets(commandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipelineLayout, offset, givenDescriptorSets.size(), vkDescriptorSetsPtr, 0, 0);
        delete[](vkDescriptorSetsPtr);
    }

    void Pipeline::BindDescriptorSet(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<PushDescriptorSet> &givenDescriptorSet) const
    {
        if (givenDescriptorSet->writeDescriptorSets.empty()) return;

        // Collect all write descriptor sets in a vector
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        writeDescriptorSets.reserve(givenDescriptorSet->writeDescriptorSets.size());
        for (const auto &writeSet : givenDescriptorSet->writeDescriptorSets)
        {
            writeDescriptorSets.push_back(writeSet.second);
        }

        // Push set
        vkCmdPushDescriptorSetKHR(commandBuffer->GetVulkanCommandBuffer(), bindPoint, vkPipelineLayout, givenDescriptorSet->GetDescriptorSetIndex(), writeDescriptorSets.size(), writeDescriptorSets.data());
    }

    void Pipeline::BindDescriptorSets(const UniquePtr<CommandBuffer> &commandBuffer, const std::vector<ReferenceWrapper<UniquePtr<PushDescriptorSet>>> &givenDescriptorSets) const
    {
        // Bind each set individually
        for (uint32 i = givenDescriptorSets.size(); i--;)
        {
            BindDescriptorSet(commandBuffer, givenDescriptorSets[i]);
        }
    }

    void Pipeline::SetPushConstants(const UniquePtr<CommandBuffer> &commandBuffer, const void* data) const
    {
        #if DEBUG
            if (pushConstantRange == nullptr)
            {
                ASSERT_WARNING("Trying to push constants to a pipeline that has none defined! Returning");
                return;
            }
        #endif

        vkCmdPushConstants(commandBuffer->GetVulkanCommandBuffer(), vkPipelineLayout, pushConstantRange->stageFlags, pushConstantRange->offset, pushConstantRange->size, data);
    }

    void Pipeline::End(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // Set bound status
        bound = false;
    }

    /* --- SETTER METHODS --- */

    void Pipeline::SetShaderDefinition(const ShaderType shaderType, const ShaderDefinition definition)
    {
        #if DEBUG
            if (!usesPrecompiledShaders)
            {
                ASSERT_WARNING("Cannot set definition [{0}] for a shader that is within a pipeline, which uses compiled shaders! Returning", definition.name);
                return;
            }
        #endif

        auto iterator = shaders.find(shaderType);
        #if DEBUG
            if (iterator == shaders.end())
            {
                ASSERT_WARNING("Trying to set definition [{0}] for a shader of type [{1}], but no shader of such type is present in pipeline! Action suspended", definition.name, static_cast<uint32>(shaderType));
                return;
            }
        #endif

        shaders[shaderType].get()->SetDefinition(definition);
        CreatePipeline();
    }

    void Pipeline::SetSpecializationConstant(const uint32 constantID, const void* data, const uint32 range)
    {
        #if DEBUG
            if (bound)
            {
                ASSERT_WARNING("Cannot set specialization constant [{0}] while pipeline is bound! Operation suspended", constantID);
                return;
            }
        #endif

        for (const auto &[shaderType, shader] : shaders)
        {
            if (shader->specializationInfo == nullptr) continue;

            for (uint32 i = shader->specializationInfo->mapEntryCount; i--;)
            {
                if (shader->specializationInfo->pMapEntries[i].constantID == constantID)
                {
                    memcpy(reinterpret_cast<char*>(const_cast<void*>(shader->specializationInfo->pData)) + shader->specializationInfo->pMapEntries[i].offset, data, range);
                    CreatePipeline();
                    return;
                }
            }
        }

        ASSERT_WARNING("Could not find a specialization constant with an ID of [{0}] within the pipeline", constantID);
    }

    void Pipeline::SetShaderBinding(const uint32 binding, const UniquePtr<Buffer> &buffer, const uint32 arrayIndex, const uint64 size, const uint64 offset)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");

        descriptorSets[currentFrame]->SetBuffer(binding, buffer, arrayIndex, size, offset);
        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderBinding(const uint32 binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, const uint32 arrayIndex)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");

        // Save image to descriptor sets
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            descriptorSets[i]->SetImage(binding, image, sampler, arrayIndex, descriptorSetLayout->get()->GetDescriptorTypeForBinding(binding));
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderBinding(const uint32 binding, const SharedPtr<Texture> &texture, const uint32 arrayIndex)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");

        // Save texture to descriptor sets
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            descriptorSets[i]->SetTexture(binding, texture, arrayIndex);
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderBinding(const uint32 binding, const SharedPtr<Cubemap> &cubemap, const uint32 arrayIndex)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");

        // Save texture to descriptor sets
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            descriptorSets[i]->SetCubemap(binding, cubemap, arrayIndex);
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderMember(const String &memberName, const UniquePtr<Buffer> &buffer, const uint32 arrayIndex)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Cannot dynamically modify information on shader members in a pipeline that uses compiled shaders");

        ShaderMember &member = GetShaderMember(memberName);
        ASSERT_ERROR_IF(member.memberType != ShaderMemberType::BUFFER, "Trying to set buffer for member [{0}], but it is not of type ShaderMemberType::BUFFER", memberName);

        // If not bound save buffer for current frame only
        member.bufferData->buffers[currentFrame] = buffer.get();
        for (const auto binding : member.descriptorData->bindings)
        {
            descriptorSets[currentFrame]->SetBuffer(binding, buffer, arrayIndex);
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderMember(const String &memberName, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, const uint32 arrayIndex, ImageLayout imageLayoutAtDrawTime)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Cannot dynamically modify information on shader members in a pipeline that uses compiled shaders");

        ShaderMember &member = GetShaderMember(memberName);
        ASSERT_ERROR_IF(member.memberType != ShaderMemberType::TEXTURE, "Trying to set image and sampler for member [{0}], but it is not of type ShaderMemberType::TEXTURE", memberName);

        // Save image in descriptor sets
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            for (const auto binding : member.descriptorData->bindings)
            {
                descriptorSets[i]->SetImage(binding, image, sampler, arrayIndex);
            }
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderMember(const String &memberName, const SharedPtr<Texture> &texture, const uint32 arrayIndex)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Cannot dynamically modify information on shader members in a pipeline that uses compiled shaders");

        ShaderMember &member = GetShaderMember(memberName);
        ASSERT_ERROR_IF(member.memberType != ShaderMemberType::TEXTURE, "Trying to set texture for member [{0}], but it is not of type ShaderMemberType::TEXTURE", memberName);

        // Save texture in descriptor sets
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            for (const auto binding : member.descriptorData->bindings)
            {
                descriptorSets[i]->SetTexture(binding, texture, arrayIndex);
            }
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderMember(const String &memberName, const SharedPtr<Cubemap> &cubemap, const uint32 arrayIndex)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Cannot dynamically modify information on shader members in a pipeline that uses compiled shaders");

        ShaderMember &member = GetShaderMember(memberName);
        ASSERT_ERROR_IF(member.memberType != ShaderMemberType::CUBEMAP, "Trying to set cubemap for member [{0}], but it is not of type ShaderMemberType::CUBEMAP", memberName);

        // Save texture in descriptor sets
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            for (const auto binding : member.descriptorData->bindings)
            {
                descriptorSets[i]->SetCubemap(binding, cubemap, arrayIndex);
            }
        }

        updatedDescriptorSetForFrame = false;
    }

    void Pipeline::SetShaderMember(const String &memberName, void* data, const uint64 size, const uint64 offset)
    {
        ASSERT_ERROR_IF(!bound, "Cannot modify shader bindings unless pipeline is bound");
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Cannot dynamically modify information on shader members in a pipeline that uses compiled shaders");

        const ShaderMember &member = GetShaderMember(memberName);
        ASSERT_ERROR_IF(bound && member.memberType != ShaderMemberType::PUSH_CONSTANT, "Cannot modify non-push-constant shader member data with this method during drawing/dispatch! To set shader members to a bound pipeline you can get the binding for the member [{0}] and create a dedicated descriptor set", memberName);

        switch (member.memberType)
        {
            case BUFFER:
            {
                if (bound)
                {
                    ASSERT_ERROR_IF(member.bufferData->buffers[currentFrame] == nullptr, "Cannot set data of shader member [{0}], as the actual corresponding buffer has not been set with SetShaderMember()", memberName);
                    member.bufferData->buffers[currentFrame]->CopyFromPointer(data, size == 0 ? member.bufferData->memorySize : size, offset);
                }
                else
                {
                    Buffer* lastBuffer = nullptr;
                    for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
                    {
                        if (lastBuffer == member.bufferData->buffers[i]) continue;

                        ASSERT_ERROR_IF(member.bufferData->buffers[i] == nullptr, "Cannot set data of shader member [{0}], as the actual corresponding buffer has not been set with SetShaderMember()", memberName);
                        member.bufferData->buffers[i]->CopyFromPointer(data, size == 0 ? member.bufferData->memorySize : size, offset);
                        lastBuffer = member.bufferData->buffers[i];
                    }
                }
                break;
            }
            case PUSH_CONSTANT:
            {
                member.pushConstantData->data->SetDataByOffset(data, size == 0 ? member.pushConstantData->memorySize : size, offset);
                break;
            }
            case SPECIALIZATION_CONSTANT:
            {
                // Recreate shaders that use it
                for (const auto &[shaderType, shader] : shaders)
                {
                    if (IS_FLAG_PRESENT(member.shaderStages, shaderType))
                    {
                        memcpy(const_cast<char*>(reinterpret_cast<const char*>(shader->specializationInfo->pData) + member.specializationConstantData->offset), data, member.specializationConstantData->size);
                        CreatePipeline();
                    }
                }
                break;
            }
            default:
            {
                ASSERT_ERROR("Cannot set plain pointer data for member [{0}], as it is neither of type ShaderMemberType::BUFFER, nor of ShaderMemberType::PUSH_CONSTANT", memberName);
            }
        }
    }

    /* --- GETTER METHODS --- */

    uint64 Pipeline::GetShaderMemberMemorySize(const String &memberName)
    {
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Cannot dynamically retrieve information on shader members in a pipeline that uses compiled shaders");

        ShaderMember &member = GetShaderMember(memberName);
        switch (member.memberType)
        {
            case BUFFER:
            {
                return member.bufferData->memorySize;
            }
            case PUSH_CONSTANT:
            {
                return member.pushConstantData->data->GetMemorySize();
            }
            default:
            {
                ASSERT_ERROR("Cannot get memory information for shader member [{0}], which is neither of type ShaderMemberType::BUFFER, nor of ShaderMemberType::PUSH_CONSTANT! Returning 0", memberName);
                return 0;
            }
        }
    }

    std::unordered_set<uint32> Pipeline::GetShaderMemberBindings(const String &memberName)
    {
        ShaderMember &member = GetShaderMember(memberName);
        ASSERT_ERROR_IF(member.memberType == ShaderMemberType::PUSH_CONSTANT, "Cannot get the binding index of member [{0}], as it is of type ShaderMemberType::PUSH_CONSTANT", memberName);
        return member.descriptorData->bindings;
    }

    Hash Pipeline::GetPipelineCacheHash() const
    {
        String stringToCache;
        for (const auto &[shaderType, shader] : shaders)
        {
            stringToCache += shader->GetFilePath();
        }

        return HashType(stringToCache);
    }

    void Pipeline::BindResources(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        if (pushConstantRange != nullptr && usesPrecompiledShaders) SetPushConstants(commandBuffer, pushConstantMemory);
        if (descriptorSetLayout != nullptr && !descriptorSets[currentFrame]->IsEmpty() && !updatedDescriptorSetForFrame)
        {
            BindDescriptorSet(commandBuffer, descriptorSets[currentFrame]);
            updatedDescriptorSetForFrame = true;
        }
    }

    String Pipeline::GetPipelineCacheFilePath() const
    {
        return Engine::Project::GetProjectDirectory() / "PipelineCache" / ("PipelineCache_" + std::to_string(GetPipelineCacheHash()) + ".cache");
    }

    ShaderMember& Pipeline::GetShaderMember(const String &memberName)
    {
        ASSERT_ERROR_IF(!usesPrecompiledShaders, "Functionality only works with pipelines, which use compiled (.spv) shaders");

        auto iterator = shaderMembers.find({ .memberNameHash = HashType(memberName) });
        ASSERT_ERROR_IF(iterator == shaderMembers.end(), "Could not find shader member [{0}] in pipeline", memberName);
        return iterator->second;
    }

    /* --- DESTRUCTOR --- */

    void Pipeline::Destroy()
    {
        // Offload shader data
        for (const auto &member : shaderMembers)
        {
            member.second.Destroy();
        }
        for (const auto &[shaderType, shader] : shaders)
        {
            shader->Destroy();
        }
        delete(pushConstantRange);

        // Destroy descriptor set layout if created automatically
        if (usesPrecompiledShaders && descriptorSetLayout != nullptr ) descriptorSetLayout->get()->Destroy();

        // If no cache was read during creation of pipeline, save new one
        if (pipelineCache == VK_NULL_HANDLE)
        {
            VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

            vkCreatePipelineCache(VK::GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache);

            uSize cacheSize;
            vkGetPipelineCacheData(VK::GetLogicalDevice(), pipelineCache, &cacheSize, nullptr);

            std::vector<uint8> cacheData(cacheSize);
            vkGetPipelineCacheData(VK::GetLogicalDevice(), pipelineCache, &cacheSize, cacheData.data());

            Engine::File::WriteBinaryDataToFile(GetPipelineCacheFilePath(), cacheData, true, true);
        }

        vkDestroyPipelineCache(VK::GetLogicalDevice(), pipelineCache, nullptr);
        pipelineCache = VK_NULL_HANDLE;

        vkDestroyPipelineLayout(VK::GetLogicalDevice(), vkPipelineLayout, nullptr);
        vkPipelineLayout = VK_NULL_HANDLE;

        vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
        vkPipeline = VK_NULL_HANDLE;
    }
}