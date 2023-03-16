//
// Created by Nikolay Kanchevski on 25.02.23.
//

#include "../UI/ImGuiCore.h"
#include "RenderingUtilities.h"
#include "../../../Engine/Classes/Cursor.h"
#include "../../../Engine/Classes/Input.h"

namespace Sierra::Core::Rendering::Vulkan
{
    /* --- POLLING METHODS --- */

    void Raycaster::UpdateData(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        using Sierra::Engine::Classes::Input;
        using Sierra::Engine::Classes::Cursor;
        using namespace Sierra::Core::Rendering::UI;

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
        
        computePipeline->Bind(commandBuffer);

        computePipeline->PushConstants(commandBuffer);
        computePipeline->BindDescriptorSets(commandBuffer);

        commandBuffer->Dispatch(1, 1, 1);

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