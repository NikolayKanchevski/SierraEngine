//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include "CommandTask.h"
#include "CommandBuffer.h"

#include "RenderPass.h"
#include "Image.h"

#include "../Core/InputManager.h"
#include "../Core/CursorManager.h"

namespace Sierra
{

    struct ImGuiFontCreateInfo
    {
        const std::filesystem::path &filePath;
        float32 size = 18.0f;
    };

    struct ImGuiStyle
    {
        float32 alpha                            = 1.0f;
        float32 disabledAlpha                    = 0.60f;
        Vector2 windowPadding                    = Vector2(8.00f, 8.00f);
        float32 windowRounding                   = 0.0f;
        float32 windowBorderSize                 = 1.0f;
        Vector2 windowMinSize                    = Vector2(32.0f, 32.0f);
        Vector2 windowTitleAlign                 = Vector2(0.00f, 0.50f);
        float32 childRounding                    = 0.0f;
        float32 childBorderSize                  = 1.0f;
        float32 popupRounding                    = 0.0f;
        float32 popupBorderSize                  = 1.0f;
        Vector2 framePadding                     = Vector2(4.00f, 3.00f);
        float32 frameRounding                    = 0.0f;
        float32 frameBorderSize                  = 0.0f;
        Vector2 itemSpacing                      = Vector2(8.00f, 4.00f);
        Vector2 itemInnerSpacing                 = Vector2(4.00f, 4.00f);
        Vector2 cellPadding                      = Vector2(4.00f, 2.00f);
        Vector2 touchExtraPadding                = Vector2(0.00f, 0.00f);
        float32 indentSpacing                    = 21.0f;
        float32 columnsMinSpacing                = 6.0f;
        float32 scrollbarSize                    = 14.0f;
        float32 scrollbarRounding                = 9.0f;
        float32 grabMinSize                      = 12.0f;
        float32 grabRounding                     = 0.0f;
        float32 tabRounding                      = 4.0f;
        float32 tabBorderSize                    = 0.0f;
        float32 tabBarBorderSize                 = 1.0f;
        Vector2 buttonTextAlign                  = Vector2(0.50f, 0.50f);
        Vector2 selectableTextAlign              = Vector2(0.00f, 0.00f);
        float32 separatorTextBorderSize          = 3.0f;
        Vector2 separatorTextAlign               = Vector2(0.00f, 0.50f);
        Vector2 separatorTextPadding             = Vector2(20.0f, 3.00f);
        Vector2 displayWindowPadding             = Vector2(19.0f, 19.0f);
        Vector2 displaySafeAreaPadding           = Vector2(3.00f, 3.00f);
        float32 dockingSeparatorSize             = 2.0f;
        bool enableAntiAliasing                  = true;
        struct {
            Color text                            = Vector4(1.00f, 1.00f, 1.00f, 1.00f);
            Color textDisabled                    = Color(0.50f, 0.50f, 0.50f, 1.00f);
            Color windowBackground                = Color(0.06f, 0.06f, 0.06f, 0.94f);
            Color childBackground                 = Color(0.00f, 0.00f, 0.00f, 0.00f);
            Color popupBackground                 = Color(0.08f, 0.08f, 0.08f, 0.94f);
            Color border                          = Color(0.43f, 0.43f, 0.50f, 0.50f);
            Color borderShadow                    = Color(0.00f, 0.00f, 0.00f, 0.00f);
            Color frameBackground                 = Color(0.16f, 0.29f, 0.48f, 0.54f);
            Color frameBackgroundHovered          = Color(0.26f, 0.59f, 0.98f, 0.40f);
            Color frameBackgroundActive           = Color(0.26f, 0.59f, 0.98f, 0.67f);
            Color titleBackground                 = Color(0.04f, 0.04f, 0.04f, 1.00f);
            Color titleBackgroundActive           = Color(0.16f, 0.29f, 0.48f, 1.00f);
            Color titleBackgroundCollapsed        = Color(0.00f, 0.00f, 0.00f, 0.51f);
            Color menuBarBackground               = Color(0.14f, 0.14f, 0.14f, 1.00f);
            Color scrollbarBackground             = Color(0.02f, 0.02f, 0.02f, 0.53f);
            Color scrollbarGrab                   = Color(0.31f, 0.31f, 0.31f, 1.00f);
            Color scrollbarGrabHovered            = Color(0.41f, 0.41f, 0.41f, 1.00f);
            Color scrollbarGrabActive             = Color(0.51f, 0.51f, 0.51f, 1.00f);
            Color checkMark                       = Color(0.26f, 0.59f, 0.98f, 1.00f);
            Color sliderGrab                      = Color(0.24f, 0.52f, 0.88f, 1.00f);
            Color sliderGrabActive                = Color(0.26f, 0.59f, 0.98f, 1.00f);
            Color button                          = Color(0.26f, 0.59f, 0.98f, 0.40f);
            Color buttonHovered                   = Color(0.26f, 0.59f, 0.98f, 1.00f);
            Color buttonActive                    = Color(0.06f, 0.53f, 0.98f, 1.00f);
            Color header                          = Color(0.26f, 0.59f, 0.98f, 0.31f);
            Color headerHovered                   = Color(0.26f, 0.59f, 0.98f, 0.80f);
            Color headerActive                    = Color(0.26f, 0.59f, 0.98f, 1.00f);
            Color separator                       = Color(0.43f, 0.43f, 0.50f, 0.50f);
            Color separatorHovered                = Color(0.10f, 0.40f, 0.75f, 0.78f);
            Color separatorActive                 = Color(0.10f, 0.40f, 0.75f, 1.00f);
            Color resizeGrip                      = Color(0.26f, 0.59f, 0.98f, 0.20f);
            Color resizeGripHovered               = Color(0.26f, 0.59f, 0.98f, 0.67f);
            Color resizeGripActive                = Color(0.26f, 0.59f, 0.98f, 0.95f);
            Color tab                             = Color(0.18f, 0.35f, 0.58f, 0.86f);
            Color tabHovered                      = Color(0.26f, 0.59f, 0.98f, 0.80f);
            Color tabActive                       = Color(0.20f, 0.40f, 0.69f, 1.00f);
            Color tabUnfocused                    = Color(0.08f, 0.19f, 0.35f, 0.86f);
            Color tabUnfocusedActive              = Color(0.14f, 0.24f, 0.39f, 1.00f);
            Color dockingPreview                  = Color(0.26f, 0.59f, 0.98f, 0.70f);
            Color dockingEmptyBackground          = Color(0.20f, 0.20f, 0.20f, 1.00f);
            Color plotLines                       = Color(0.61f, 0.61f, 0.61f, 1.00f);
            Color plotLinesHovered                = Color(1.00f, 0.43f, 0.35f, 1.00f);
            Color plotHistogram                   = Color(0.90f, 0.70f, 0.00f, 1.00f);
            Color plotHistogramHovered            = Color(1.00f, 0.60f, 0.00f, 1.00f);
            Color tableHeaderBackground           = Color(0.19f, 0.19f, 0.20f, 1.00f);
            Color tableBorderStrong               = Color(0.31f, 0.31f, 0.35f, 1.00f);
            Color tableBorderLight                = Color(0.23f, 0.23f, 0.25f, 1.00f);
            Color tableRowBackground              = Color(0.00f, 0.00f, 0.00f, 0.00f);
            Color tableRowBackgroundAlt           = Color(1.00f, 1.00f, 1.00f, 0.06f);
            Color textSelectedBackground          = Color(0.26f, 0.59f, 0.98f, 0.35f);
            Color dragDropTarget                  = Color(1.00f, 1.00f, 0.00f, 0.90f);
            Color navHighlight                    = Color(0.26f, 0.59f, 0.98f, 1.00f);
            Color navWindowingHighlight           = Color(1.00f, 1.00f, 1.00f, 0.70f);
            Color navWindowingDimBackground       = Color(0.80f, 0.80f, 0.80f, 0.20f);
            Color modalWindowDimBackground        = Color(0.80f, 0.80f, 0.80f, 0.35f);
        } colors;
    };

    struct ImGuiRenderTaskCreateInfo
    {
        const ImGuiStyle &style = { };
        const std::initializer_list<ImGuiFontCreateInfo> &fontCreateInfos = { };

        const std::unique_ptr<Image> &templateImage;
        float32 scaling = 1.0f;
    };

    class SIERRA_API ImGuiRenderTask : public CommandTask
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void BeginFrame(const InputManager &inputManager, const CursorManager &cursorManager);
        void EndFrame();

        virtual void Resize(uint32 width, uint32 height);
        virtual void Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image) = 0;

        /* --- DESTRUCTOR --- */
        virtual ~ImGuiRenderTask();

    protected:
        explicit ImGuiRenderTask(const ImGuiRenderTaskCreateInfo &createInfo);
        [[nodiscard]] inline uint32 GetActiveTaskCount() const { return activeTaskCount; }

    private:
        void* style; // ::ImGuiStyle
        Vector2 framebufferSize = { };

        static inline void* context = nullptr; // ImGuiContext*
        static inline uint32 activeTaskCount = 0;

    };

}
