//
// Created by Nikolay Kanchevski on 24.06.23.
//

#include "Raycaster.h"

#include "../RenderingSettings.h"
#include "../../../Editor/Editor.h"
#include "../../../Engine/Classes/Cursor.h"
#include "../../../Engine/Handlers/Project.h"

#define OUTPUT_BUFFER_BINDING 1
#define ID_BUFFER_BINDING 2
#define DEPTH_BUFFER_BINDING 3

namespace Sierra::Rendering::Modules
{

    /* --- CONSTRUCTORS --- */

    Raycaster::Raycaster(const RaycasterCreateInfo &createInfo)
        : IDBuffer(createInfo.IDBuffer), depthBuffer(createInfo.depthBuffer), sampler(createInfo.sampler)
    {
        // Create the buffer where the compute pipeline will be outputting the data
        outputBuffer = Buffer::Create({ .memorySize = sizeof(RaycasterOutputData), .bufferUsage = BufferUsage::STORAGE });

        // Create descriptor set layout to use
        descriptorSetLayout = DescriptorSetLayout::Create({
            .bindings = {
                { UNIFORM_BUFFER_BINDING, { .descriptorType = DescriptorType::UNIFORM_BUFFER,         .shaderStages = ShaderType::COMPUTE } },
                { OUTPUT_BUFFER_BINDING,  { .descriptorType = DescriptorType::STORAGE_BUFFER,         .shaderStages = ShaderType::COMPUTE } },
                { ID_BUFFER_BINDING,      { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::COMPUTE } },
                { DEPTH_BUFFER_BINDING,   { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::COMPUTE } },
            },
            .flags = DescriptorSetLayoutFlag::PUSH_DESCRIPTOR_KHR
        });

        // Load shader
        auto computeShader = Shader::Load({ .filePath = Engine::Project::GetProjectDirectory() / "Shaders/Raycasting/Raycast.comp.spv", .shaderType = ShaderType::COMPUTE });

        // Create compute pipeline
        computePipeline = ComputePipeline::Create({
            .shader = computeShader,
            .shaderInfo = CompiledPipelineShaderInfo {
                .pushConstantData = PushConstantData {
                    .size = sizeof(PushConstant),
                    .shaderStages = ShaderType::COMPUTE
                },
                .descriptorSetLayout = &descriptorSetLayout
            }
        });
    }

    UniquePtr<Raycaster> Raycaster::Create(const RaycasterCreateInfo &createInfo)
    {
        return std::make_unique<Raycaster>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void Raycaster::UpdateData(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<Buffer> &uniformBuffer)
    {
        commandBuffer->SynchronizeImageUsage(IDBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, ImageLayout::SHADER_READ_ONLY_OPTIMAL);
        commandBuffer->SynchronizeImageUsage(depthBuffer, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Set mouse position, which will later be used as UV in shader
        PushConstant pushConstant{};
        if (Window::IsFocusedWindowPresent())
        {
            Vector2 mousePositionWithinView = Engine::Cursor::GetGlfwCursorPosition();
            mousePositionWithinView.x -= Editor::GetSceneViewPositionX();
            mousePositionWithinView.x /= Editor::GetSceneViewWidth();
            mousePositionWithinView.y -= Editor::GetSceneViewPositionY();
            mousePositionWithinView.y /= Editor::GetSceneViewHeight();
            pushConstant.mousePosition = mousePositionWithinView;
        }
        else
        {
            pushConstant.mousePosition = { -1, -1 };
        }

        // Start pipeline
        computePipeline->Begin(commandBuffer);

        // Set shader data
        computePipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffer);
        computePipeline->SetShaderBinding(OUTPUT_BUFFER_BINDING, outputBuffer);
        computePipeline->SetShaderBinding(ID_BUFFER_BINDING, IDBuffer, sampler);
        computePipeline->SetShaderBinding(DEPTH_BUFFER_BINDING, depthBuffer, sampler);
        computePipeline->SetPushConstants(commandBuffer, pushConstant);

        // Execute compute shader
        computePipeline->Dispatch(commandBuffer, 1);

        commandBuffer->SynchronizeImageUsage(IDBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, ImageLayout::SHADER_READ_ONLY_OPTIMAL);
        commandBuffer->SynchronizeImageUsage(depthBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, ImageLayout::SHADER_READ_ONLY_OPTIMAL);
        commandBuffer->SynchronizeBufferUsage(outputBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT);
    }

    /* --- DESTRUCTOR --- */

    void Raycaster::Destroy()
    {
        outputBuffer->Destroy();
        descriptorSetLayout->Destroy();
        computePipeline->Destroy();
    }

}