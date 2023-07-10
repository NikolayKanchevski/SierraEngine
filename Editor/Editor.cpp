//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "Editor.h"

#include "UIPanels.h"
#include "../Core/World.h"
#include "../Engine/Components/Camera.h"

namespace Sierra
{
    /* --- PROPERTIES --- */
    float sceneViewWidth = 1.0f, sceneViewHeight = 1.0f;
    float xSceneViewPosition = 1.0f, ySceneViewPosition = 1.0f;

    ImGuiStyle defaultStyle;

    /* --- POLLING METHODS --- */

    void Editor::DrawEditor(const PerFrameEngineEditorData &perFrameData)
    {
        MainViewportPanel::DrawUI();
        auto rendererViewportOutput = RendererViewportPanel::DrawUI({ .imGuiInstance = perFrameData.imGuiInstance, .renderedTextureDescriptorSet = perFrameData.renderedTextureDescriptorSet });
        HierarchyPanel::DrawUI();
        PropertiesPanel::DrawUI();
        DebugPanel::DrawUI({ .frameDrawTime = perFrameData.frameDrawTime });
        DetailedDebugPanel::DrawUI({ .frameDrawTime = perFrameData.frameDrawTime });
        GamePadDebugPanel::DrawUI();

        // Check if a resize was made
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

}