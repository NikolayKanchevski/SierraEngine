//
// Created by Nikolay Kanchevski on 11.05.24.
//

#include "EditorRenderer.h"

#include "../Editor/Data/Fonts.h"
#include "../Editor/Data/Themes.h"

#include "../Editor/MenuBar.h"
#include "../Editor/HierarchyPanel.h"
#include "../Editor/RenderPanel.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorRenderer::EditorRenderer(const EditorCreateInfo &createInfo)
        : imGuiRenderer({
            .style = createInfo.theme == EditorTheme::Light ? GetLightThemeStyle() : GetDarkThemeStyle(),
            .fontCreateInfos = {{
                { .size = 7.0f, .ttfMemory = INTER_FONT_TTF }
            }},
            .concurrentFrameCount = createInfo.concurrentFrameCount,
            .renderingContext = createInfo.renderingContext,
            .commandBuffer = createInfo.commandBuffer,
            .scaling = createInfo.scaling,
            .sampling = Sierra::ImageSampling::x1,
            .templateOutputImage = createInfo.templateOutputImage,
            .fontAtlasIndex = 0,
            .fontSamplerIndex = 0,
            .resourceTable = createInfo.resourceTable
        })
    {

    }

    /* --- POLLING METHODS --- */

    void EditorRenderer::Update(Scene &scene, const Sierra::Image &renderedImage, const Sierra::InputManager* inputManager, const Sierra::CursorManager* cursorManager, const Sierra::TouchManager* touchManager)
    {
        imGuiRenderer.Update(inputManager, cursorManager, touchManager);

        // Create ImGui layout
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

            constexpr static bool DRAW_MENU_BAR = true;
            ImGui::Begin("Viewport", nullptr, DRAW_MENU_BAR * ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize  | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::PopStyleVar(2);

            ImGui::DockSpace(ImGui::GetID("Dockspace"), { 0.0f, 0.0f }, ImGuiDockNodeFlags_PassthruCentralNode);
            if constexpr (DRAW_MENU_BAR) MenuBar::Draw();
            ImGui::End();
        }

        // Draw UI
        HierarchyPanel::Draw(scene, selectedEntity);
        RenderPanel::Draw(renderedImage);
    }

    void EditorRenderer::Resize(const uint32 width, const uint32 height)
    {
        imGuiRenderer.Resize(width, height);
    }

    void EditorRenderer::Render(Sierra::CommandBuffer &commandBuffer, const Sierra::Image &outputImage)
    {
        imGuiRenderer.Render(commandBuffer, outputImage);
    }

}
