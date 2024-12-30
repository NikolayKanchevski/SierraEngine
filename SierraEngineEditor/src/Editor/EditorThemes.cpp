//
// Created by Nikolay Kanchevski on 6.11.24.
//

#include "EditorThemes.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] ImGuiStyle CreateDefaultStyle() noexcept
        {
            ImGuiStyle style = { };
            ImGui::StyleColorsClassic(&style);

            style.Alpha                       = 1.0f;
            style.DisabledAlpha               = 0.50f;
            style.WindowPadding               = ImVec2(8.00f, 8.00f);
            style.WindowRounding              = 0.0f;
            style.WindowBorderSize            = 1.0f;
            style.WindowMinSize               = ImVec2(32.00f, 32.00f);
            style.WindowTitleAlign            = ImVec2(0.00f, 0.50f);
            style.WindowMenuButtonPosition    = ImGuiDir_Left;
            style.ChildRounding               = 4.0f;
            style.ChildBorderSize             = 1.0f;
            style.PopupRounding               = 4.0f;
            style.PopupBorderSize             = 1.0f;
            style.FramePadding                = ImVec2(7.50f, 3.50f);
            style.FrameRounding               = 3.0f;
            style.FrameBorderSize             = 1.0f;
            style.ItemSpacing                 = ImVec2(6.00f, 6.00f);
            style.ItemInnerSpacing            = ImVec2(6.00f, 6.00f);
            style.CellPadding                 = ImVec2(6.00f, 3.00f);
            style.TouchExtraPadding           = ImVec2(0.00f, 0.00f);
            style.IndentSpacing               = 25.0f;
            style.ColumnsMinSpacing           = 6.0f;
            style.ScrollbarSize               = 15.0f;
            style.ScrollbarRounding           = 9.0f;
            style.GrabMinSize                 = 10.0f;
            style.GrabRounding                = 3.0f;
            style.LogSliderDeadzone           = 4.0f;
            style.TabRounding                 = 4.0f;
            style.TabBorderSize               = 1.0f;
            style.TabMinWidthForCloseButton   = 0.0f;
            style.TabBarBorderSize            = 1.0f;
            style.TableAngledHeadersAngle     = 35.0f * (glm::pi<float32>() / 180.0f);
            style.TableAngledHeadersTextAlign = ImVec2(0.50f, 0.00f);
            style.ColorButtonPosition         = ImGuiDir_Right;
            style.ButtonTextAlign             = ImVec2(0.50f, 0.50f);
            style.SelectableTextAlign         = ImVec2(0.00f, 0.00f);
            style.SeparatorTextBorderSize     = 3.0f;
            style.SeparatorTextAlign          = ImVec2(0.00f, 0.50f);
            style.SeparatorTextPadding        = ImVec2(20.0f, 3.00f);
            style.DisplayWindowPadding        = ImVec2(8.00f, 8.00f);
            style.DisplaySafeAreaPadding      = ImVec2(3.00f, 3.00f);
            style.DockingSeparatorSize        = 2.0f;
            style.MouseCursorScale            = 1.0f;
            style.AntiAliasedLines            = true;
            style.AntiAliasedLinesUseTex      = true;
            style.AntiAliasedFill             = true;
            style.CurveTessellationTol        = 1.25f;
            style.CircleTessellationMaxError  = 0.30f;
            style.HoverStationaryDelay        = 0.15f;
            style.HoverDelayShort             = 0.15f;
            style.HoverDelayNormal            = 0.40f;
            style.HoverFlagsForTooltipMouse   = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled;
            style.HoverFlagsForTooltipNav     = ImGuiHoveredFlags_NoSharedDelay | ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled;

            return style;
        }

        void ApplyLightThemeToStyle(ImGuiStyle& style) noexcept
        {
            ImGui::StyleColorsLight(&style);
        }

        void ApplyDarkThemeToStyle(ImGuiStyle& style) noexcept
        {
            style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
            style.Colors[ImGuiCol_Border]                = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
            style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
            style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
            style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            style.Colors[ImGuiCol_Button]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            style.Colors[ImGuiCol_Header]                = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
            style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
            style.Colors[ImGuiCol_Separator]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            style.Colors[ImGuiCol_Tab]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
            style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_NavHighlight]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
            style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
            style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
        }
    }

    ImGuiStyle EditorThemeToImGuiStyle(const EditorTheme theme)
    {
        ImGuiStyle style = CreateDefaultStyle();
        switch (theme)
        {
            case EditorTheme::Light:    { ApplyLightThemeToStyle(style); break; }
            case EditorTheme::Dark:     { ApplyDarkThemeToStyle(style); break; }
            default:                    break;
        }

        return style;
    }

}