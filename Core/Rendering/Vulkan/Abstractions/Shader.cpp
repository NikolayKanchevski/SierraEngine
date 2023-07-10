//
// Created by Nikolay Kanchevski on 18.12.22.
//

#include "Shader.h"

#include "../VK.h"
#include "../../RenderingSettings.h"
#include "../../../../Engine/Classes/File.h"

namespace Sierra::Rendering
{
    std::unordered_map<String, String> Shader::defaultDefinitions =
    {
        {
            "MAX_MESHES",
            std::to_string(MAX_MESHES)
        },
        {
            "MAX_TEXTURES",
            std::to_string(MAX_TEXTURES)
        },
        {
            "MAX_DIRECTIONAL_LIGHTS",
            std::to_string(MAX_DIRECTIONAL_LIGHTS)
        },
        {
            "MAX_POINT_LIGHTS",
            std::to_string(MAX_POINT_LIGHTS)
        }
    };

    /* --- CONSTRUCTORS --- */

    Shader::Shader(const ShaderCreateInfo &createInfo)
        : filePath(createInfo.filePath), shaderType(createInfo.shaderType != ShaderType::NONE ? createInfo.shaderType : GetShaderTypeFromExtension())
    {
        // Save data
        precompiledData = new PrecompiledData();
        precompiledData->optimization = createInfo.optimization;

        // Add definitions to shader
        precompiledData->definitions = defaultDefinitions;

        for (const auto &pair : createInfo.definitions)
            precompiledData->definitions[pair.name] = pair.value;

        // Compile shader to SPIRV
        auto code = CompileShadercShader();

        // Generate reflection data for a shader
        SpvReflectShaderModule module;
        SpvReflectResult result = spvReflectCreateShaderModule(code.size() * UINT_SIZE, code.data(), &module);
        ASSERT_ERROR_FORMATTED_IF(result != SPV_REFLECT_RESULT_SUCCESS, "Could not create reflection module for shader [{0}]", filePath);

        // Gather reflection data
        SaveReflectionData(module);

        // Create shader module
        CreateShaderModule(DEFAULT_ENTRY_POINT, code);

        // Destroy reflection module
        spvReflectDestroyShaderModule(&module);
    }

    SharedPtr<Shader> Shader::Create(const ShaderCreateInfo &createInfo)
    {
        return std::make_shared<Shader>(createInfo);
    }

    Shader::Shader(const CompiledShaderCreateInfo &createInfo)
        : filePath(createInfo.filePath), shaderType(createInfo.shaderType)
    {
        ASSERT_ERROR_IF(createInfo.shaderType == ShaderType::NONE, "Shader type must be specified for compiled to binary shaders");

        // Setup specialization constants
        if (!createInfo.specializationConstants.empty())
        {
            // Allocate specialization info
            specializationInfo = new VkSpecializationInfo();
            specializationInfo->pMapEntries = new VkSpecializationMapEntry[createInfo.specializationConstants.size()];

            // Set memory
            uint i = 0;
            uint lastOffset = 0;
            for (const auto &[constantID, specializationConstantData] : createInfo.specializationConstants)
            {
                // Set entry info
                const_cast<VkSpecializationMapEntry*>(specializationInfo->pMapEntries)[i].constantID = constantID;
                const_cast<VkSpecializationMapEntry*>(specializationInfo->pMapEntries)[i].size = specializationConstantData.size;
                const_cast<VkSpecializationMapEntry*>(specializationInfo->pMapEntries)[i].offset = lastOffset;

                // Update offset
                lastOffset += specializationConstantData.size;
                i++;
            }

            // Populate data
            specializationInfo->dataSize = lastOffset;
            specializationInfo->mapEntryCount = createInfo.specializationConstants.size();

            // Allocate data memory
            Engine::MemoryObject specializationData(lastOffset, 1);
            specializationInfo->pData = specializationData.GetData();
        }

        // Set up vertex attributes
        if (!createInfo.vertexAttributes.empty())
        {
            #if DEBUG
                if (shaderType != ShaderType::VERTEX)
                {
                    ASSERT_WARNING("Do not pass vertex attributes for shaders that are not of type ShaderType::VERTEX");
                }
            #endif

            // Allocate attribute data
            vertexAttributes = new VkVertexInputAttributeDescription[createInfo.vertexAttributes.size()];

            // Populate attributes
            uint lastOffset = 0;
            for (uint i = 0; i < createInfo.vertexAttributes.size(); i++)
            {
                vertexAttributes[i].binding = 0;
                vertexAttributes[i].location = i;
                vertexAttributes[i].format = static_cast<VkFormat>(createInfo.vertexAttributes[i]);
                vertexAttributes[i].offset = lastOffset;

                vertexAttributeCount = i + 1;
                lastOffset += GetVertexAttributeTypeSize(createInfo.vertexAttributes[i]);
            }
        }

        // Create shader module
        auto code = Engine::File::ReadFile(filePath);
        CreateShaderModule(createInfo.entryPoint, code);
    }

    SharedPtr<Shader> Shader::Load(const CompiledShaderCreateInfo &createInfo)
    {
        return std::make_shared<Shader>(createInfo);
    }

    /* --- SETTER METHODS --- */

    std::vector<uint> Shader::CompileShadercShader()
    {
        // Set options
        shaderc::CompileOptions options;
        options.SetGenerateDebugInfo();
        options.SetIncluder(std::make_unique<Includer>(filePath));

        options.SetForcedVersionProfile(450, shaderc_profile_none);
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetTargetEnvironment(shaderc_target_env_vulkan, VK_VERSION);
        options.SetTargetSpirv(shaderc_spirv_version_1_5);

        options.SetAutoSampledTextures(true);
        options.SetAutoMapLocations(true);
        options.SetAutoBindUniforms(true);
        options.SetPreserveBindings(false);

        uint startingBindingForStage = MAX_BINDINGS_PER_STAGE * GetShadercShaderType();
        options.SetBindingBase(shaderc_uniform_kind_image, startingBindingForStage);
        options.SetBindingBase(shaderc_uniform_kind_sampler, startingBindingForStage);
        options.SetBindingBase(shaderc_uniform_kind_texture, startingBindingForStage);
        options.SetBindingBase(shaderc_uniform_kind_buffer, startingBindingForStage);
        options.SetBindingBase(shaderc_uniform_kind_storage_buffer, startingBindingForStage);
        options.SetBindingBase(shaderc_uniform_kind_unordered_access_view, startingBindingForStage);

        #if DEBUG
            options.SetWarningsAsErrors();
        #endif

        // Define macros
        for (const auto &pair : precompiledData->definitions)
        {
            options.AddMacroDefinition(pair.first, pair.second);
        }

        // Set the optimization
        if (precompiledData->optimization != ShaderOptimization::None) options.SetOptimizationLevel((shaderc_optimization_level) precompiledData->optimization);

        // Read and compile shader
        auto shaderCode = Engine::File::ReadFile(filePath);
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderCode.data(), shaderCode.size(), GetShadercShaderType(), Engine::File::GetFileNameFromPath(filePath).c_str(), options);

        // Check for errors
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ASSERT_ERROR_FORMATTED("Precompiling shader [{0}] failed with an error code of [{1}]:\n\t{2}", filePath, static_cast<uint>(module.GetCompilationStatus()), module.GetErrorMessage());
        }

        return { module.begin(), module.end() };
    }

    void Shader::SaveReflectionData(const SpvReflectShaderModule &reflectionModule)
    {
        // Vertex attributes
        if (shaderType == ShaderType::VERTEX && reflectionModule.input_variable_count > 0)
        {
            precompiledData->reflectionData.vertexAttributes = new std::vector<VertexAttribute>();

            // Retrieve attributes
            auto &vertexAttributeOutput = *precompiledData->reflectionData.vertexAttributes;
            for (uint i = 0; i < reflectionModule.input_variable_count; i++)
            {
                const auto &inputVariable = reflectionModule.input_variables[i];

                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
                if (inputVariable->built_in != -1) continue;
                #pragma clang diagnostic pop

                vertexAttributeOutput.push_back({ .location = inputVariable->location, .vertexAttributeType = static_cast<VertexAttributeType>(inputVariable->format) });
            }

            // Sort attributes by location
            std::sort(vertexAttributeOutput.begin(), vertexAttributeOutput.end(),
            [](const VertexAttribute &left, const VertexAttribute &right)
            {
                return left.location < right.location;
            });
        }

        // Allocate & reserve memory for members
        precompiledData->reflectionData.members = new ShaderMembers();
        precompiledData->reflectionData.members->reserve(reflectionModule.descriptor_binding_count + (reflectionModule.push_constant_block_count != 0) + reflectionModule.specialization_constant_count);

        // Descriptor bindings
        for (uint i = 0; i < reflectionModule.descriptor_binding_count; i++)
        {
            auto &descriptorBinding = reflectionModule.descriptor_bindings[i];
            auto &descriptorBindingOutputData = (*precompiledData->reflectionData.members)[{
                #if DEBUG
                    .memberName = String(descriptorBinding.name),
                    .typeName = descriptorBinding.type_description->type_name != nullptr ? String(descriptorBinding.type_description->type_name) : std::to_string(static_cast<uint>(descriptorBinding.descriptor_type)),
                #endif
                .memberNameHash = HashType(String(descriptorBinding.name))
            }];

            // Load base binding data
            descriptorBindingOutputData.descriptorData = new ShaderDescriptorData();
            descriptorBindingOutputData.shaderStages = shaderType;
            descriptorBindingOutputData.descriptorData->bindings = { descriptorBinding.binding };
            descriptorBindingOutputData.descriptorData->set = descriptorBinding.set;
            descriptorBindingOutputData.descriptorData->type = static_cast<DescriptorType>(descriptorBinding.descriptor_type);
            for (uint j = descriptorBinding.array.dims_count; j--;)
            {
                descriptorBindingOutputData.descriptorData->arraySize *= descriptorBinding.array.dims[j];
            }

            // Allocate data
            switch (descriptorBinding.descriptor_type)
            {
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                {
                    // Set member type and allocate data
                    descriptorBindingOutputData.memberType = ShaderMemberType::BUFFER;
                    descriptorBindingOutputData.bufferData = new ShaderBufferData();

                    // Load root members only
                    for (uint j = 0; j < descriptorBinding.block.member_count; j++)
                    {
                        // Get references to binding data
                        auto &inputBlock = descriptorBinding.block.members[j];
                        auto &outputBlock = *descriptorBindingOutputData.bufferData;

                        // Copy member data
                        outputBlock.memorySize = inputBlock.padded_size;
                    }

                    break;
                }
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                {
                    // Set member type
                    descriptorBindingOutputData.memberType = ShaderMemberType::TEXTURE;
                    break;
                }
                default:
                {
                    ASSERT_ERROR_FORMATTED("Variables of type [{0}] are not supported in precompiled shaders", static_cast<uint>(descriptorBinding.descriptor_type));
                    break;
                }
            }
        }

        // Push constants
        if (reflectionModule.push_constant_block_count > 1)
        {
            ASSERT_ERROR_FORMATTED("Shader [{0}] has {1} push constant blocks! Vulkan only allows one", filePath, reflectionModule.push_constant_block_count);
        }
        else
        {
            if (reflectionModule.push_constant_block_count != 0)
            {
                const auto &pushConstantBlock = reflectionModule.push_constant_blocks[0];
                auto &pushConstantOutputData = (*precompiledData->reflectionData.members)[{
                    #if DEBUG
                        .memberName = String(pushConstantBlock.name).c_str(),
                        .typeName = String(pushConstantBlock.type_description->type_name).c_str(),
                    #endif
                    .memberNameHash = HashType(String(pushConstantBlock.name))
                }];

                ASSERT_ERROR_FORMATTED_IF(pushConstantBlock.size > 128, "Vulkan requires all push constants to be of a size no bigger than 128 bytes! This shader's push constant has a size of {0} bytes", pushConstantBlock.size);

                // Initialize value for push constant data
                pushConstantOutputData.shaderStages = shaderType;
                pushConstantOutputData.memberType = ShaderMemberType::PUSH_CONSTANT;
                pushConstantOutputData.pushConstantData = new ShaderPushConstantData();
                pushConstantOutputData.pushConstantData->memorySize = pushConstantBlock.padded_size;
            }
        }

        // Specialization constants preparation
        Engine::MemoryObject specializationData;
        uint specializationDataSize = 0;
        if (reflectionModule.specialization_constant_count != 0)
        {
            // Calculate total specialization size
            for (uint i = reflectionModule.specialization_constant_count; i--;)
            {
                specializationDataSize += reflectionModule.specialization_constants[i].size;
            }

            // Allocate specialization data
            specializationData = Engine::MemoryObject(specializationDataSize, 1);

            // Allocate specialization info memory
            specializationInfo = new VkSpecializationInfo();
            specializationInfo->dataSize = specializationDataSize;
            specializationInfo->mapEntryCount = reflectionModule.specialization_constant_count;
            specializationInfo->pMapEntries = new VkSpecializationMapEntry[reflectionModule.specialization_constant_count];
            specializationInfo->pData = specializationData.GetData();
        }

        // Specialization constants
        uint lastOffset = 0;
        for (uint i = 0; i < reflectionModule.specialization_constant_count; i++)
        {
            const auto &specializationConstantBlock = reflectionModule.specialization_constants[i];
            auto &outputSpecializationOutputData = (*precompiledData->reflectionData.members)[{
                #if DEBUG
                    .memberName = String(specializationConstantBlock.name).c_str(),
                    .typeName = std::to_string(specializationConstantBlock.constant_type).c_str(),
                #endif
                .memberNameHash = HashType(String(specializationConstantBlock.name))
            }];

            // Initialize & fill out specialization constant data
            outputSpecializationOutputData.shaderStages = shaderType;
            outputSpecializationOutputData.memberType = ShaderMemberType::SPECIALIZATION_CONSTANT;

            outputSpecializationOutputData.specializationConstantData = new ShaderSpecializationConstantData();
            outputSpecializationOutputData.specializationConstantData->size = specializationConstantBlock.size;
            outputSpecializationOutputData.specializationConstantData->offset = lastOffset;
            specializationData.SetDataByOffset(&specializationConstantBlock.default_value.float_value, lastOffset, specializationConstantBlock.size);

            // Update specialization info
            const_cast<VkSpecializationMapEntry*>(specializationInfo->pMapEntries)->constantID = specializationConstantBlock.constant_id;
            const_cast<VkSpecializationMapEntry*>(specializationInfo->pMapEntries)->size = specializationConstantBlock.size;
            const_cast<VkSpecializationMapEntry*>(specializationInfo->pMapEntries)->offset = lastOffset;

            // Calculate offset
            lastOffset += specializationConstantBlock.size;
        }
    }

    bool Shader::SetDefinition(const ShaderDefinition &definition)
    {
        precompiledData->definitions[definition.name] = definition.value;

        auto code = CompileShadercShader();
        CreateShaderModule(DEFAULT_ENTRY_POINT, code);

        return true;
    }

    ShaderType Shader::GetShaderTypeFromExtension()
    {
        if (filePath.rfind(".vert", filePath.size() - 1) != String::npos) return ShaderType::VERTEX;
        if (filePath.rfind(".frag", filePath.size() - 1) != String::npos) return ShaderType::FRAGMENT;
        if (filePath.rfind(".geom", filePath.size() - 1) != String::npos) return ShaderType::GEOMETRY;
        if (filePath.rfind(".tese", filePath.size() - 1) != String::npos) return ShaderType::TESSELATION_EVALUATION;
        if (filePath.rfind(".tesc", filePath.size() - 1) != String::npos) return ShaderType::TESSELATION_CONTROL;
        if (filePath.rfind(".comp", filePath.size() - 1) != String::npos) return ShaderType::COMPUTE;
        if (filePath.rfind(".glsl", filePath.size() - 1) != String::npos) ASSERT_ERROR("Cannot determine the type of a shader file with extension of [.glsl]");

        return ShaderType::NONE;
    }

    shaderc_shader_kind Shader::GetShadercShaderType()
    {
        switch (shaderType)
        {
            case ShaderType::VERTEX:
                return shaderc_shader_kind::shaderc_glsl_vertex_shader;
            case ShaderType::FRAGMENT:
                return shaderc_shader_kind::shaderc_glsl_fragment_shader;
            case ShaderType::GEOMETRY:
                return shaderc_shader_kind::shaderc_glsl_geometry_shader;
            case ShaderType::TESSELATION_EVALUATION:
                return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
            case ShaderType::TESSELATION_CONTROL:
                return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
            case ShaderType::COMPUTE:
                return shaderc_shader_kind::shaderc_glsl_compute_shader;
            default:
                ASSERT_ERROR("Cannot determine the shaderc::shader_type of a [ShaderType::NONE]");
        }

        // A random value to eliminate compiler warning
        return shaderc_task_shader;
    }

    /* --- DESTRUCTORS --- */

    void Shader::Destroy()
    {
        if (specializationInfo != nullptr)
        {
            if (specializationInfo->pData != nullptr)
            {
                free(const_cast<void*>(specializationInfo->pData));
            }
            delete[](specializationInfo->pMapEntries);
        }

        delete(specializationInfo);
        delete(shaderStageCreateInfo);

        if (precompiledData != nullptr) delete(precompiledData->reflectionData.vertexAttributes);
        delete(precompiledData);

        vkDestroyShaderModule(VK::GetLogicalDevice(), shaderModule, nullptr);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint> || std::is_same_v<T, char>, bool>>
    void Shader::CreateShaderModule(const char *entryPoint, const std::vector<T> &code)
    {
        // Destroy module if already created
        if (shaderModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(VK::GetLogicalDevice(), shaderModule, nullptr);
        }

        // Set module creation info
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = code.size() * sizeof(T);
        moduleCreateInfo.pCode = reinterpret_cast<const uint*>(code.data());

        // Create shader module
        VK_ASSERT(
            vkCreateShaderModule(VK::GetLogicalDevice(), &moduleCreateInfo, nullptr, &shaderModule),
            FORMAT_STRING("Failed to create shader module for [{0}]", filePath)
        );

        // Setup shader stage creation info
        shaderStageCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo->stage = static_cast<VkShaderStageFlagBits>(shaderType);
        shaderStageCreateInfo->module = shaderModule;
        shaderStageCreateInfo->pName = entryPoint;
        shaderStageCreateInfo->pSpecializationInfo = specializationInfo;
    }

    /* --- INCLUDER --- */

    Shader::Includer::Includer(String givenFilePath)
        : filePath(std::move(givenFilePath))
    {

    }

    shaderc_include_result* Shader::Includer::GetInclude(const char* requestedSource, const shaderc_include_type type, const char* requestingSource, const size_t includeDepth)
    {
        // Get which file is requested to be included
        const std::string includeReference = std::string(requestedSource);

        // Gather its data
        auto fileData = Engine::File::ReadBinaryFile(Engine::File::RemoveFileNameFromPath(filePath) + includeReference);
        String includedContents = String(fileData.begin(), fileData.end());

        // Assign the pointers of the read data to a container
        auto container = new std::array<std::string, 2>();
        (*container)[0] = includeReference;
        (*container)[1] = includedContents;

        // Set the remaining data
        auto data = new shaderc_include_result;
        data->user_data = container;
        data->source_name = (*container)[0].data();
        data->source_name_length = (*container)[0].size();
        data->content = (*container)[1].data();
        data->content_length = (*container)[1].size();

        return data;
    }

    void Shader::Includer::ReleaseInclude(shaderc_include_result *data)
    {
        // Deallocate the container allocated in GetInclude()
        delete(static_cast<std::array<std::string, 2>*>(data->user_data));
        delete(data);
    }
}
