//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "../../Core/Rendering/UI/ImGuiInstance.h"

namespace Sierra::Editor::RendererViewportPanel
{
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

    /* --- POLLING METHODS --- */
    [[nodiscard]] RendererViewportPanelOutput DrawUI(const RendererViewportPanelInput &input);
}