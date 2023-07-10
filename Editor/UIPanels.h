//
// Created by Nikolay Kanchevski on 17.02.23.
//

#pragma once

#include "../Core/Rendering/UI/ImGuiInstance.h"

namespace Sierra::Editor
{

    namespace MainViewportPanel
    {
        void DrawUI();
    }

    struct RendererViewportPanelInput
    {
        UniquePtr<Rendering::ImGuiInstance> &imGuiInstance;
        VkDescriptorSet renderedTextureDescriptorSet = VK_NULL_HANDLE;
    };

    struct RendererViewportPanelOutput
    {
        float xSceneViewPosition = 0;
        float ySceneViewPosition = 0;

        float sceneViewWidth = 0;
        float sceneViewHeight = 0;
    };

    namespace RendererViewportPanel
    {
        [[nodiscard]] RendererViewportPanelOutput DrawUI(const RendererViewportPanelInput &input);
    }

    namespace HierarchyPanel
    {
        void DrawUI();
    }

    namespace PropertiesPanel
    {
        void DrawUI();
    }

    struct DebugPanelInput
    {
        float frameDrawTime = 0.0f;
    };

    namespace DebugPanel
    {
        void DrawUI(const DebugPanelInput &input);
    }

    namespace DetailedDebugPanel
    {
        void DrawUI(const DebugPanelInput &input);
    }

    namespace GamePadDebugPanel
    {
        void DrawUI();
    }

}
