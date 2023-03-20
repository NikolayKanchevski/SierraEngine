//
// Created by Nikolay Kanchevski on 25.02.23.
//

#include "../UI/ImGuiCore.h"
#include "RenderingUtilities.h"
#include "../../../Engine/Classes/Cursor.h"
#include "../../../Engine/Classes/Input.h"

namespace Sierra::Core::Rendering::Vulkan
{

    // ========================== GRID RENDERER ========================== \\

    /* --- POLLING METHODS --- */

    void GridRenderer::RenderGrid(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // Make sure image is suitable for being rendering to
        commandBuffer->TransitionImageLayout(outputImage, ImageLayout::COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        // Begin rendering
        renderer->Begin(commandBuffer);

        // Fire the pipeline up
        pipeline->Bind(commandBuffer);

        // Pass data to shaders
        pipeline->BindDescriptorSets(commandBuffer);

        // Draw grid plane
        commandBuffer->Draw(6);

        // End rendering
        renderer->End(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    void GridRenderer::Destroy()
    {
        pipeline->Destroy();
        renderer->Destroy();
    }

    // ========================== RAYCASTER ========================== \\

    /* --- POLLING METHODS --- */

    void Raycaster::UpdateData(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        using Sierra::Engine::Classes::Input;
        using Sierra::Engine::Classes::Cursor;
        using namespace Sierra::Core::Rendering::UI;

        // Set mouse position, which will later be used as UV in shader
        if (Window::IsFocusedWindowPresent())
        {
            Vector2 mousePositionWithinView = Cursor::GetGlfwCursorPosition();
            mousePositionWithinView.x -= ImGuiCore::GetSceneViewPositionX();
            mousePositionWithinView.x /= ImGuiCore::GetSceneViewWidth();
            mousePositionWithinView.y -= ImGuiCore::GetSceneViewPositionY();
            mousePositionWithinView.y /= ImGuiCore::GetSceneViewHeight();
            mousePositionWithinView.y = 1.0f - mousePositionWithinView.y;
            computePipeline->GetPushConstantData().mousePosition = mousePositionWithinView;
        }
        else
        {
            computePipeline->GetPushConstantData().mousePosition = { -1, -1 };
        }

        // Start pipeline
        computePipeline->Bind(commandBuffer);

        // Pass data to compute shader
        computePipeline->PushConstants(commandBuffer);
        computePipeline->BindDescriptorSets(commandBuffer);

        // Execute compute shader
        commandBuffer->Dispatch(1, 1, 1);

        // Save new data
        data = *dataBuffer->GetDataAs<WriteBuffer>();
        data.worldPosition.y *= -1;
    }

    /* --- DESTRUCTOR --- */

    void Raycaster::Destroy()
    {
        computePipeline->Destroy();
        dataBuffer->Destroy();
    }

}