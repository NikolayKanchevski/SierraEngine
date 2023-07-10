//
// Created by Nikolay Kanchevski on 29.06.23.
//

#pragma once

#include "../../Core/Rendering/UI/ImGuiInstance.h"

namespace Sierra::Editor
{

    struct PerFrameEngineEditorData
    {
        UniquePtr<Rendering::ImGuiInstance> &imGuiInstance;
        VkDescriptorSet renderedTextureDescriptorSet = VK_NULL_HANDLE;
        float frameDrawTime = 0.0f;
    };

    /* --- POLLING METHODS --- */
    void DrawEditor(const PerFrameEngineEditorData &perFrameData);

    /* --- GETTER METHODS --- */
    float GetSceneViewPositionX();
    float GetSceneViewPositionY();

    float GetSceneViewWidth();
    float GetSceneViewHeight();

}
