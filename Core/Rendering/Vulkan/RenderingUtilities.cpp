//
// Created by Nikolay Kanchevski on 25.02.23.
//

#include "../UI/ImGuiCore.h"
#include "RenderingUtilities.h"
#include "../../../Engine/Classes/Cursor.h"
#include "../../../Engine/Classes/Input.h"

namespace Sierra::Core::Rendering::Vulkan
{
    RenderingUtilities RenderingUtilities::instance;

    /* --- POLLING METHODS --- */

    bool RenderingUtilities::UpdateIDBuffer(const UniquePtr<CommandBuffer> &commandBuffer)
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
            instance.IDBufferComputePipeline->GetPushConstantData().mousePosition = mousePositionWithinView;
        }
        else
        {
            instance.IDBufferComputePipeline->GetPushConstantData().mousePosition = { -1, -1 };
        }

        instance.IDBufferComputePipeline->Bind(commandBuffer);

        instance.IDBufferComputePipeline->PushConstants(commandBuffer);
        instance.IDBufferComputePipeline->BindDescriptorSets(commandBuffer, instance.IDBufferDescriptorSet);

        commandBuffer->Dispatch(1, 1, 1);

        instance.IDData = *instance.IDBuffer->GetDataAs<IDWriteBuffer>();

        return true;
    }

    /* --- DESTRUCTOR --- */

    void RenderingUtilities::Destroy()
    {
        instance.IDBufferDescriptorSetLayout->Destroy();
        instance.IDBufferComputePipeline->Destroy();
        instance.IDBuffer->Destroy();
    }

}