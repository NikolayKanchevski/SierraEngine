//
// Created by Nikolay Kanchevski on 18.12.22.
//

#pragma once

#include "../VulkanTypes.h"
#include "../../../../Engine/Classes/MemoryObject.h"
#include "Buffer.h"
#include "Texture.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- DATA TYPES --- */

    struct ShaderDefinition
    {
        String name;
        String value = "1";
    };

    struct ShaderSpecializationConstant
    {
        uint size;
    };

    enum ShaderOptimization
    {
        None = shaderc_optimization_level_zero,
        OptimizeSize = shaderc_optimization_level_size,
        OptimizePerformance = shaderc_optimization_level_performance
    };

    DEFINE_ENUM_FLAG_OPERATORS(ShaderOptimization)

    struct CompiledShaderCreateInfo
    {
        String filePath;
        ShaderType shaderType;

        // Constant ID | Data
        std::unordered_map<uint, ShaderSpecializationConstant> specializationConstants;
        std::vector<VertexAttributeType> vertexAttributes;

        const char* entryPoint = "main";
    };

    struct ShaderCreateInfo
    {
        String filePath;
        ShaderType shaderType = ShaderType::NONE;

        std::vector<ShaderDefinition> definitions;

        #if DEBUG
            ShaderOptimization optimization = ShaderOptimization::None;
        #else
            ShaderOptimization optimization = ShaderOptimization::OptimizeSize | ShaderOptimization::OptimizePerformance;
        #endif
    };

    enum ShaderMemberType
    {
        BUFFER = 0,
        PUSH_CONSTANT = 1,
        SPECIALIZATION_CONSTANT = 2,
        TEXTURE = 3,
        CUBEMAP = 4
    };

    struct ShaderMemberBlockKey
    {
        #if DEBUG
            String name;
            String type;
        #endif
        Hash nameHash;

        struct KeyHash
        {
            inline Hash operator()(const ShaderMemberBlockKey &key) const
            {
                return key.nameHash;
            }
        };

        struct KeyEquality
        {
            inline bool operator()(const ShaderMemberBlockKey &left, const ShaderMemberBlockKey &right) const
            {
                return left.nameHash == right.nameHash;
            }
        };
    };

    struct ShaderMemberBlock
    {
        typedef std::unordered_map<ShaderMemberBlockKey, ShaderMemberBlock, ShaderMemberBlockKey::KeyHash, ShaderMemberBlockKey::KeyEquality> ShaderMemberBlockTable;

        uint absoluteOffset;
        uint memorySize;
        uint arraySize = 1;
        ShaderMemberBlock::ShaderMemberBlockTable childrenBlocks;

        [[nodiscard]] inline uint GetStride() const
        {
            return memorySize / arraySize;
        }
    };

    struct ShaderDescriptorData
    {
        uint set;
        // We use a set of uints here, as if other shaders in pipeline use the resource on different bindings, they will have to be put here
        std::unordered_set<uint> bindings;
        uint arraySize = 1;
        DescriptorType type;
    };

    struct ShaderBufferData
    {
        uint memorySize;
        std::vector<Buffer*> buffers;
    };

    struct ShaderPushConstantData
    {
        uint memorySize;
        UniquePtr<Engine::Classes::MemoryObject> data;
    };

    struct ShaderSpecializationConstantData
    {
        uint size;
        uint offset;
    };

    struct ShaderTextureData
    {

    };

    struct ShaderCubemapData
    {

    };

    struct ShaderMemberKey
    {
        #if DEBUG
            String memberName;
            String typeName;
        #endif

        Hash memberNameHash;

        struct KeyHash
        {
            inline Hash operator()(const ShaderMemberKey &key) const
            {
                return key.memberNameHash;
            }
        };

        struct KeyEquality
        {
            inline bool operator()(const ShaderMemberKey &left, const ShaderMemberKey &right) const
            {
                return left.memberNameHash == right.memberNameHash;
            }
        };
    };

    struct ShaderMember
    {
        ShaderMemberType memberType;
        ShaderType shaderStages;

        ShaderDescriptorData* descriptorData = nullptr;
        ShaderBufferData* bufferData = nullptr;
        ShaderPushConstantData* pushConstantData = nullptr;
        ShaderSpecializationConstantData* specializationConstantData = nullptr;

        inline void Destroy() const
        {
            switch (memberType)
            {
                case ShaderMemberType::BUFFER:
                {
                    delete(bufferData);
                    delete(descriptorData);
                    break;
                }
                case ShaderMemberType::PUSH_CONSTANT:
                {
                    pushConstantData->data->Destroy();
                    delete(pushConstantData);
                    break;
                }
                case ShaderMemberType::SPECIALIZATION_CONSTANT:
                {
                    delete(specializationConstantData);
                    break;
                }
                case TEXTURE:
                {
                    delete(descriptorData);
                    break;
                }
                case ShaderMemberType::CUBEMAP:
                {
                    delete(descriptorData);
                    break;
                }
            }
        }
    };

    class Shader
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Shader(const CompiledShaderCreateInfo &createInfo);
        static SharedPtr<Shader> Load(const CompiledShaderCreateInfo &createInfo);

        explicit Shader(const ShaderCreateInfo &createInfo);
        static SharedPtr<Shader> Create(const ShaderCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline String GetFilePath() const { return filePath; }
        [[nodiscard]] inline ShaderType GetShaderType() const { return shaderType; }
        [[nodiscard]] inline bool IsPrecompiled() const { return precompiledData != nullptr; };
        [[nodiscard]] inline auto* GetPrecompiledData() const { ASSERT_WARNING_IF(!IsPrecompiled(), "Getting precompiled data from a non-precompiled shader may lead to issues! A null pointer was returned"); return precompiledData; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Shader);

        friend class Pipeline;
        friend class GraphicsPipeline;
        friend class ComputePipeline;

        typedef std::unordered_map<ShaderMemberKey, ShaderMember, ShaderMemberKey::KeyHash, ShaderMemberKey::KeyEquality> ShaderMembers;

    private:
        String filePath;
        ShaderType shaderType;

        VkShaderModule shaderModule = VK_NULL_HANDLE;

        VkPipelineShaderStageCreateInfo* shaderStageCreateInfo = new VkPipelineShaderStageCreateInfo();
        VkSpecializationInfo* specializationInfo = nullptr;

        uint vertexAttributeCount = 0;
        VkVertexInputAttributeDescription* vertexAttributes = nullptr;

        struct VertexAttribute
        {
            uint location;
            VertexAttributeType vertexAttributeType;
        };

        struct ReflectionData
        {
            // These are manually freed by pipeline
            std::vector<VertexAttribute>* vertexAttributes;
            ShaderMembers* members;
        };

        struct PrecompiledData
        {
            ShaderOptimization optimization;

            // Name | Value
            std::unordered_map<String, String> definitions;
            ReflectionData reflectionData;
        };

        PrecompiledData* precompiledData = nullptr;

        std::vector<uint> CompileShadercShader();
        template<typename T, ENABLE_IF(std::is_same_v<T, uint> || std::is_same_v<T, char>)>
        void CreateShaderModule(const char* entryPoint, const std::vector<T> &code);
        void SaveReflectionData(const SpvReflectShaderModule &reflectionModule);

    private:
        bool SetDefinition(const ShaderDefinition &definition);

        ShaderType GetShaderTypeFromExtension();
        shaderc_shader_kind GetShadercShaderType();

        static std::unordered_map<String, String> defaultDefinitions;
        inline static shaderc::Compiler compiler;

        inline static constexpr const char* DEFAULT_ENTRY_POINT = "main";
        inline static constexpr uint MAX_BINDINGS_PER_SHADER = 65535;
        inline static constexpr uint MAX_BINDINGS_PER_STAGE = MAX_BINDINGS_PER_SHADER / (shaderc_tess_evaluation_shader + 1);

        class Includer : public shaderc::CompileOptions::IncluderInterface
        {
        public:
            explicit Includer(String  givenFilePath);

            shaderc_include_result* GetInclude(const char* requestedSource, shaderc_include_type type, const char* requestingSource, size_t includeDepth) override;
            void ReleaseInclude(shaderc_include_result* data) override;

        private:
            String filePath;

        };
    };

}