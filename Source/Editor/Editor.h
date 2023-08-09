//
// Created by Nikolay Kanchevski on 29.06.23.
//

#pragma once

#include "../Engine/Classes/Asset.h"
#include "../Core/Rendering/UI/ImGuiInstance.h"

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

    /* --- SETTER METHODS --- */
    void SetSelectedEntity(const Engine::Entity &entity);
    void SetSelectedAsset(const Engine::AssetID &asset);

    /* --- GETTER METHODS --- */
    float GetSceneViewPositionX();
    float GetSceneViewPositionY();

    float GetSceneViewWidth();
    float GetSceneViewHeight();

    Engine::Entity& GetSelectedEntity();
    Engine::AssetID& GetSelectedAsset();
}
