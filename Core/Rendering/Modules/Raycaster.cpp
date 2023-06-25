//
// Created by Nikolay Kanchevski on 24.06.23.
//

#include "Raycaster.h"

#include "../UI/ImGuiCore.h"
#include "../RenderingSettings.h"
#include "../../../Engine/Classes/File.h"
#include "../../../Engine/Classes/Input.h"
#include "../../../Engine/Classes/Cursor.h"

#define OUTPUT_BUFFER_BINDING 1
#define ID_BUFFER_BINDING 2
#define DEPTH_BUFFER_BINDING 3

using Sierra::Engine::Classes::Input;
using Sierra::Engine::Classes::Cursor;
using namespace Sierra::Core::Rendering::UI;

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Modules
{

    /* --- CONSTRUCTORS --- */

    Raycaster::Raycaster(const RaycasterCreateInfo &createInfo)
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
        auto computeShader = Shader::Load({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Raycasting/Raycast.comp.spv", .shaderType = ShaderType::COMPUTE });

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

        // Set constant shader data
        computePipeline->SetShaderBinding(OUTPUT_BUFFER_BINDING, outputBuffer);
        computePipeline->SetShaderBinding(ID_BUFFER_BINDING, createInfo.IDBuffer);
        computePipeline->SetShaderBinding(DEPTH_BUFFER_BINDING, createInfo.depthBuffer);
    }

    UniquePtr<Raycaster> Raycaster::Create(const RaycasterCreateInfo &createInfo)
    {
        return std::make_unique<Raycaster>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void Raycaster::UpdateData(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<Buffer> &uniformBuffer)
    {
        // Set mouse position, which will later be used as UV in shader
        PushConstant pushConstant{};
        if (Window::IsFocusedWindowPresent())
        {
            Vector2 mousePositionWithinView = Cursor::GetGlfwCursorPosition();
            mousePositionWithinView.x -= ImGuiCore::GetSceneViewPositionX();
            mousePositionWithinView.x /= ImGuiCore::GetSceneViewWidth();
            mousePositionWithinView.y -= ImGuiCore::GetSceneViewPositionY();
            mousePositionWithinView.y /= ImGuiCore::GetSceneViewHeight();
            mousePositionWithinView.y = 1.0f - mousePositionWithinView.y;
            pushConstant.mousePosition = mousePositionWithinView;
        }
        else
        {
            pushConstant.mousePosition = { -1, -1 };
        }

        // Start pipeline
        computePipeline->Bind(commandBuffer);

        // Set dynamic shader data
        computePipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffer);
        computePipeline->SetPushConstants(commandBuffer, pushConstant);

        // Execute compute shader
        computePipeline->Dispatch(commandBuffer, 1);
    }

    /* --- DESTRUCTOR --- */

    void Raycaster::Destroy()
    {
        outputBuffer->Destroy();
        descriptorSetLayout->Destroy();
        computePipeline->Destroy();
    }

}
