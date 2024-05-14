//
// Created by Nikolay Kanchevski on 11.05.24.
//

#include "Editor.h"

#include "Data/Fonts.h"
#include "Data/Themes.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Editor::Editor(const EditorCreateInfo &createInfo)
        : imGuiRenderTask({
            .style = createInfo.theme == EditorTheme::Light ? GetLightThemeStyle() : GetDarkThemeStyle(),
            .fontCreateInfos = {{
                { .ttfMemory = LATO_FONT_TTF }
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

    void Editor::Update(Scene &scene, const std::optional<std::reference_wrapper<const Sierra::InputManager>> &inputManager, const std::optional<std::reference_wrapper<const Sierra::CursorManager>> &cursorManager, const std::optional<std::reference_wrapper<const Sierra::TouchManager>> &touchManager)
    {
        imGuiRenderTask.Update(inputManager, cursorManager, touchManager);
        ImGui::ShowDemoWindow();
    }

    void Editor::Resize(const uint32 width, const uint32 height, const uint32 scaling)
    {
        imGuiRenderTask.Resize(width, height, scaling);
    }

    void Editor::Render(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer, const std::unique_ptr<Sierra::Image> &outputImage)
    {
        imGuiRenderTask.Render(commandBuffer, outputImage);
    }

}