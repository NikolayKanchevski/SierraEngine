//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "Editor.h"

#include "Panels/DebugPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/DiscordDebugPanel.h"
#include "Panels/MainViewportPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Components/Camera.h"
#include "Panels/RendererViewportPanel.h"

namespace Sierra
{
    /* --- PROPERTIES --- */
    float sceneViewWidth = 1.0f, sceneViewHeight = 1.0f;
    float xSceneViewPosition = 1.0f, ySceneViewPosition = 1.0f;

    Engine::AssetID selectedAsset = Engine::AssetID::Null;
    Engine::Entity selectedEntity = Engine::Entity::Null;

    /* --- POLLING METHODS --- */

    void Editor::DrawEditor(const PerFrameEngineEditorData &perFrameData)
    {
        MainViewportPanel::DrawUI();
        HierarchyPanel::DrawUI();
        PropertiesPanel::DrawUI();
        AssetBrowserPanel::DrawUI();
        DebugPanel::DrawUI({ .frameDrawTime = perFrameData.frameDrawTime });
        DiscordDebugPanel::DrawUI();

        // Check if a resize was made
        auto rendererViewportOutput = RendererViewportPanel::DrawUI({ .imGuiInstance = perFrameData.imGuiInstance, .renderedTextureDescriptorSet = perFrameData.renderedTextureDescriptorSet });
        bool resized = sceneViewWidth != rendererViewportOutput.sceneViewWidth || sceneViewHeight != rendererViewportOutput.sceneViewHeight;

        // Save new dimensions & position
        xSceneViewPosition = rendererViewportOutput.xSceneViewPosition;
        ySceneViewPosition = rendererViewportOutput.ySceneViewPosition;
        sceneViewWidth = rendererViewportOutput.sceneViewWidth;
        sceneViewHeight = rendererViewportOutput.sceneViewHeight;

        // Recalculate cameras if resized
        if (resized)
        {
            // Recalculate projection matrices
            auto cameraEntities = Engine::World::GetAllComponentsOfType<Engine::Camera>();
            for (const auto &cameraEntity : cameraEntities)
            {
                Engine::World::GetComponent<Engine::Camera>(cameraEntity).CalculateProjectionMatrix();
            }
        }
    }

    /* --- SETTER METHODS --- */

    void Editor::SetSelectedEntity(const Engine::Entity &entity)
    {
        selectedAsset = Engine::AssetID::Null;
        selectedEntity = entity;
    }

    void Editor::SetSelectedAsset(const Engine::AssetID &asset)
    {
        selectedAsset = asset;
    }

    /* --- GETTER METHODS --- */

    float Editor::GetSceneViewPositionX()
    {
        return xSceneViewPosition;
    }

    float Editor::GetSceneViewPositionY()
    {
        return ySceneViewPosition;
    }

    float Editor::GetSceneViewWidth()
    {
        return sceneViewWidth;
    }

    float Editor::GetSceneViewHeight()
    {
        return sceneViewHeight;
    }

    Engine::Entity& Editor::GetSelectedEntity()
    {
        return selectedEntity;
    }

    Engine::AssetID& Editor::GetSelectedAsset()
    {
        return selectedAsset;
    }
}