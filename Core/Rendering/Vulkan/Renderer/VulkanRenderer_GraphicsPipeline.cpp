//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"
#include "../Abstractions/Shader.h"
#include "../../../../Engine/Structures/Vertex.h"
#include "../../../../Engine/Classes/Mesh.h"

using Sierra::Engine::Classes::MeshPushConstant;

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateGraphicsPipeline()
    {
        // Create shaders
        auto vertexShader = Shader::Create({ .filePath = VulkanCore::GetDescriptorIndexingSupported() ? "Shaders/vertex_shader_bindless.vert.spv" : "Shaders/vertex_shader.vert.spv", .shaderType = VERTEX_SHADER });
        auto fragmentShader = Shader::Create({ .filePath = VulkanCore::GetDescriptorIndexingSupported() ? "Shaders/fragment_shader_bindless.frag.spv" : "Shaders/fragment_shader.frag.spv", .shaderType = FRAGMENT_SHADER });

        // Generate push constant range for MeshPushConstant
        auto pushConstantRange = CreatePushConstantRange<MeshPushConstant>();

        // Create pipeline
        graphicsPipeline = Pipeline::Create({
            .maxConcurrentFrames = maxConcurrentFrames,
            .shaders = { vertexShader, fragmentShader },
            .descriptorSetLayout = descriptorSetLayout,
            .renderPass = offscreenRenderer->GetRenderPass(),
            .pushConstantRange = &pushConstantRange,
            .sampling = msaaSampleCount,
            .shadingType = renderingMode == Fill ? SHADE_FILL : SHADE_WIREFRAME
        });
    }

}