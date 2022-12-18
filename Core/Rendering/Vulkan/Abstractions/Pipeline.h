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

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    typedef enum FrontFace
    {
        FRONT_FACE_CLOCKWISE = VK_FRONT_FACE_CLOCKWISE,
        FRONT_FACE_COUNTER_CLOCKWISE = VK_FRONT_FACE_COUNTER_CLOCKWISE
    } FrontFace;

    typedef enum CullMode
    {
        CULL_NONE = VK_CULL_MODE_NONE,
        CULL_FRONT = VK_CULL_MODE_FRONT_BIT,
        CULL_BACK = VK_CULL_MODE_BACK_BIT,
        CULL_FRONT_AND_BACK = VK_CULL_MODE_FRONT_AND_BACK
    } CullMode;

    typedef enum ShadingType
    {
        SHADE_FILL = VK_POLYGON_MODE_FILL,
        SHADE_WIREFRAME = VK_POLYGON_MODE_LINE
    } ShadingType;

    struct PipelineCreateInfo
    {
        uint32_t maxConcurrentFrames;

        std::vector<std::shared_ptr<Shader>> shaders;
        const std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout;
        const std::unique_ptr<RenderPass> &renderPass;
        VkPushConstantRange *pushConstantRange = nullptr;

        Sampling sampling = MSAAx1;
        bool createDepthBuffer = true;

        FrontFace frontFace = FRONT_FACE_COUNTER_CLOCKWISE;
        CullMode cullMode = CULL_FRONT;
        ShadingType shadingType = SHADE_FILL;
    };

    class Pipeline {
    public:
        /* --- CONSTRUCTORS --- */
        Pipeline(const PipelineCreateInfo &givenCreateInfo);
        static std::unique_ptr<Pipeline> Create(PipelineCreateInfo createInfo);

        /* --- SETTER METHODS --- */
        void Bind(VkCommandBuffer givenCommandBuffer);
        void BindDescriptorSets(VkCommandBuffer givenCommandBuffer, const std::vector<VkDescriptorSet> descriptorSets);
        void PushConstants(VkCommandBuffer givenCommandBuffer, const void *givenData);

        void OverloadShader(std::shared_ptr<Shader> newShader);

        /* --- DESTRUCTOR --- */
        void Destroy();
        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;

    private:
        VkPipelineLayout vkPipelineLayout;
        VkPipeline vkPipeline;

        VkPushConstantRange *pushConstantRange = nullptr;
        PipelineCreateInfo createInfo;

        bool alreadyCreated = false;

        void CreatePipelineLayout();
        void CreatePipeline();

    };

    template<typename T>
    VkPushConstantRange CreatePushConstantRange(ShaderType shaderStages = VERTEX_SHADER | FRAGMENT_SHADER, uint32_t offset = 0)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.size = sizeof(T);
        pushConstantRange.offset = offset;
        pushConstantRange.stageFlags = shaderStages;

        return pushConstantRange;
    }

}
