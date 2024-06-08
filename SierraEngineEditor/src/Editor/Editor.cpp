//
// Created by Nikolay Kanchevski on 11.05.24.
//

#include "Editor.h"

#include "Data/Fonts.h"
#include "Data/Themes.h"

#include "Panels/Viewport.h"
#include "Panels/Hierarchy.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Editor::Editor(const EditorCreateInfo &createInfo)
        : imGuiRenderTask({
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

    void Editor::Update(Scene &scene, const Sierra::InputManager* inputManager, const Sierra::CursorManager* cursorManager, const Sierra::TouchManager* touchManager)
    {
        imGuiRenderTask.Update(inputManager, cursorManager, touchManager);

        Viewport::Draw();
        Hierarchy::Draw(scene, selectedEntity);
    }

    void Editor::Resize(const uint32 width, const uint32 height, const uint32 scaling)
    {
        imGuiRenderTask.Resize(width, height, scaling);
    }

    void Editor::Render(Sierra::CommandBuffer &commandBuffer, const Sierra::Image &outputImage)
    {
        imGuiRenderTask.Render(commandBuffer, outputImage);
    }

}