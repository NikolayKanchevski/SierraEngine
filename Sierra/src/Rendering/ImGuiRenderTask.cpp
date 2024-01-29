//
// Created by Nikolay Kanchevski on 28.01.24.
//

#include "ImGuiRenderTask.h"

#define IM_VEC2_CLASS_EXTRA                                                                       \
		ImVec2(const Vector2 &other) { x = other.x; y = other.y; }                                \
		operator Vector2() const { return Vector2(x, y); }

#define IM_VEC4_CLASS_EXTRA                                                                       \
		ImVec4(const Vector4 &other) { x = other.x; y = other.y; z = other.z; w = other.w; }      \
		operator Vector4() const { return Vector4(x, y, z, w); }

#include <imgui.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ImGuiRenderTask::ImGuiRenderTask(const ImGuiRenderTaskCreateInfo &createInfo)
        : framebufferSize({ static_cast<float32>(createInfo.templateImage->GetWidth()) / createInfo.scaling, static_cast<float32>(createInfo.templateImage->GetHeight()) / createInfo.scaling })
    {
        if (activeTaskCount == 0)
        {
            // Create context
            context = ImGui::CreateContext();
            ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(context));

            // Configure context
            ImGuiIO &io = ImGui::GetIO();
            io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | (SR_PLATFORM_MOBILE * ImGuiConfigFlags_IsTouchScreen);
            io.BackendFlags = ImGuiBackendFlags_RendererHasVtxOffset;
            io.DisplayFramebufferScale = { createInfo.scaling, createInfo.scaling };
        }

        activeTaskCount++;

        // Convert style
        ::ImGuiStyle* nativeStyle = new ::ImGuiStyle();
        nativeStyle->Alpha                                         = createInfo.style.alpha;
        nativeStyle->DisabledAlpha                                 = createInfo.style.disabledAlpha;
        nativeStyle->WindowPadding                                 = createInfo.style.windowPadding;
        nativeStyle->WindowRounding                                = createInfo.style.windowRounding;
        nativeStyle->WindowBorderSize                              = createInfo.style.windowBorderSize;
        nativeStyle->WindowMinSize                                 = createInfo.style.windowMinSize;
        nativeStyle->WindowTitleAlign                              = createInfo.style.windowTitleAlign;
        nativeStyle->ChildRounding                                 = createInfo.style.childRounding;
        nativeStyle->ChildBorderSize                               = createInfo.style.childBorderSize;
        nativeStyle->PopupRounding                                 = createInfo.style.popupRounding;
        nativeStyle->PopupBorderSize                               = createInfo.style.popupBorderSize;
        nativeStyle->FramePadding                                  = createInfo.style.framePadding;
        nativeStyle->FrameRounding                                 = createInfo.style.frameRounding;
        nativeStyle->FrameBorderSize                               = createInfo.style.frameBorderSize;
        nativeStyle->ItemSpacing                                   = createInfo.style.itemSpacing;
        nativeStyle->ItemInnerSpacing                              = createInfo.style.itemInnerSpacing;
        nativeStyle->CellPadding                                   = createInfo.style.cellPadding;
        nativeStyle->TouchExtraPadding                             = createInfo.style.touchExtraPadding;
        nativeStyle->IndentSpacing                                 = createInfo.style.indentSpacing;
        nativeStyle->ColumnsMinSpacing                             = createInfo.style.columnsMinSpacing;
        nativeStyle->ScrollbarSize                                 = createInfo.style.scrollbarSize;
        nativeStyle->ScrollbarRounding                             = createInfo.style.scrollbarRounding;
        nativeStyle->GrabMinSize                                   = createInfo.style.grabMinSize;
        nativeStyle->GrabRounding                                  = createInfo.style.grabRounding;
        nativeStyle->TabRounding                                   = createInfo.style.tabRounding;
        nativeStyle->TabBorderSize                                 = createInfo.style.tabBorderSize;
        nativeStyle->TabBarBorderSize                              = createInfo.style.tabBarBorderSize;
        nativeStyle->ButtonTextAlign                               = createInfo.style.buttonTextAlign;
        nativeStyle->SelectableTextAlign                           = createInfo.style.selectableTextAlign;
        nativeStyle->SeparatorTextBorderSize                       = createInfo.style.separatorTextBorderSize;
        nativeStyle->SeparatorTextAlign                            = createInfo.style.separatorTextAlign;
        nativeStyle->SeparatorTextPadding                          = createInfo.style.separatorTextPadding;
        nativeStyle->DisplayWindowPadding                          = createInfo.style.displayWindowPadding;
        nativeStyle->DisplaySafeAreaPadding                        = createInfo.style.displaySafeAreaPadding;
        nativeStyle->DockingSeparatorSize                          = createInfo.style.dockingSeparatorSize;
        nativeStyle->AntiAliasedLines                              = createInfo.style.enableAntiAliasing;
        nativeStyle->AntiAliasedLinesUseTex                        = createInfo.style.enableAntiAliasing;
        nativeStyle->AntiAliasedFill                               = createInfo.style.enableAntiAliasing;
        nativeStyle->Colors[ImGuiCol_Text]                         = createInfo.style.colors.text;
        nativeStyle->Colors[ImGuiCol_TextDisabled]                 = createInfo.style.colors.textDisabled;
        nativeStyle->Colors[ImGuiCol_WindowBg]                     = createInfo.style.colors.windowBackground;
        nativeStyle->Colors[ImGuiCol_ChildBg]                      = createInfo.style.colors.childBackground;
        nativeStyle->Colors[ImGuiCol_PopupBg]                      = createInfo.style.colors.popupBackground;
        nativeStyle->Colors[ImGuiCol_Border]                       = createInfo.style.colors.border;
        nativeStyle->Colors[ImGuiCol_BorderShadow]                 = createInfo.style.colors.borderShadow;
        nativeStyle->Colors[ImGuiCol_FrameBg]                      = createInfo.style.colors.frameBackground;
        nativeStyle->Colors[ImGuiCol_FrameBgHovered]               = createInfo.style.colors.frameBackgroundHovered;
        nativeStyle->Colors[ImGuiCol_FrameBgActive]                = createInfo.style.colors.frameBackgroundActive;
        nativeStyle->Colors[ImGuiCol_TitleBg]                      = createInfo.style.colors.titleBackground;
        nativeStyle->Colors[ImGuiCol_TitleBgActive]                = createInfo.style.colors.titleBackgroundActive;
        nativeStyle->Colors[ImGuiCol_TitleBgCollapsed]             = createInfo.style.colors.titleBackgroundCollapsed;
        nativeStyle->Colors[ImGuiCol_MenuBarBg]                    = createInfo.style.colors.menuBarBackground;
        nativeStyle->Colors[ImGuiCol_ScrollbarBg]                  = createInfo.style.colors.scrollbarBackground;
        nativeStyle->Colors[ImGuiCol_ScrollbarGrab]                = createInfo.style.colors.scrollbarGrab;
        nativeStyle->Colors[ImGuiCol_ScrollbarGrabHovered]         = createInfo.style.colors.scrollbarGrabHovered;
        nativeStyle->Colors[ImGuiCol_ScrollbarGrabActive]          = createInfo.style.colors.scrollbarGrabActive;
        nativeStyle->Colors[ImGuiCol_CheckMark]                    = createInfo.style.colors.checkMark;
        nativeStyle->Colors[ImGuiCol_SliderGrab]                   = createInfo.style.colors.sliderGrab;
        nativeStyle->Colors[ImGuiCol_SliderGrabActive]             = createInfo.style.colors.sliderGrabActive;
        nativeStyle->Colors[ImGuiCol_Button]                       = createInfo.style.colors.button;
        nativeStyle->Colors[ImGuiCol_ButtonHovered]                = createInfo.style.colors.buttonHovered;
        nativeStyle->Colors[ImGuiCol_ButtonActive]                 = createInfo.style.colors.buttonActive;
        nativeStyle->Colors[ImGuiCol_Header]                       = createInfo.style.colors.header;
        nativeStyle->Colors[ImGuiCol_HeaderHovered]                = createInfo.style.colors.headerHovered;
        nativeStyle->Colors[ImGuiCol_HeaderActive]                 = createInfo.style.colors.headerActive;
        nativeStyle->Colors[ImGuiCol_Separator]                    = createInfo.style.colors.separator;
        nativeStyle->Colors[ImGuiCol_SeparatorHovered]             = createInfo.style.colors.separatorHovered;
        nativeStyle->Colors[ImGuiCol_SeparatorActive]              = createInfo.style.colors.separatorActive;
        nativeStyle->Colors[ImGuiCol_ResizeGrip]                   = createInfo.style.colors.resizeGrip;
        nativeStyle->Colors[ImGuiCol_ResizeGripHovered]            = createInfo.style.colors.resizeGripHovered;
        nativeStyle->Colors[ImGuiCol_ResizeGripActive]             = createInfo.style.colors.resizeGripActive;
        nativeStyle->Colors[ImGuiCol_Tab]                          = createInfo.style.colors.tab;
        nativeStyle->Colors[ImGuiCol_TabHovered]                   = createInfo.style.colors.tabHovered;
        nativeStyle->Colors[ImGuiCol_TabActive]                    = createInfo.style.colors.tabActive;
        nativeStyle->Colors[ImGuiCol_TabUnfocused]                 = createInfo.style.colors.tabUnfocused;
        nativeStyle->Colors[ImGuiCol_TabUnfocusedActive]           = createInfo.style.colors.tabUnfocusedActive;
        nativeStyle->Colors[ImGuiCol_DockingPreview]               = createInfo.style.colors.dockingPreview;
        nativeStyle->Colors[ImGuiCol_DockingEmptyBg]               = createInfo.style.colors.dockingEmptyBackground;
        nativeStyle->Colors[ImGuiCol_PlotLines]                    = createInfo.style.colors.plotLines;
        nativeStyle->Colors[ImGuiCol_PlotLinesHovered]             = createInfo.style.colors.plotLinesHovered;
        nativeStyle->Colors[ImGuiCol_PlotHistogram]                = createInfo.style.colors.plotHistogram;
        nativeStyle->Colors[ImGuiCol_PlotHistogramHovered]         = createInfo.style.colors.plotHistogramHovered;
        nativeStyle->Colors[ImGuiCol_TableHeaderBg]                = createInfo.style.colors.tableHeaderBackground;
        nativeStyle->Colors[ImGuiCol_TableBorderStrong]            = createInfo.style.colors.tableBorderStrong;
        nativeStyle->Colors[ImGuiCol_TableBorderLight]             = createInfo.style.colors.tableBorderLight;
        nativeStyle->Colors[ImGuiCol_TableRowBg]                   = createInfo.style.colors.tableRowBackground;
        nativeStyle->Colors[ImGuiCol_TableRowBgAlt]                = createInfo.style.colors.tableRowBackgroundAlt;
        nativeStyle->Colors[ImGuiCol_TextSelectedBg]               = createInfo.style.colors.textSelectedBackground;
        nativeStyle->Colors[ImGuiCol_DragDropTarget]               = createInfo.style.colors.dragDropTarget;
        nativeStyle->Colors[ImGuiCol_NavHighlight]                 = createInfo.style.colors.navHighlight;
        nativeStyle->Colors[ImGuiCol_NavWindowingHighlight]        = createInfo.style.colors.navWindowingHighlight;
        nativeStyle->Colors[ImGuiCol_NavWindowingDimBg]            = createInfo.style.colors.navWindowingDimBackground;
        nativeStyle->Colors[ImGuiCol_ModalWindowDimBg]             = createInfo.style.colors.modalWindowDimBackground;
        style = nativeStyle;
    }

    /* --- POLLING METHODS --- */

    void ImGuiRenderTask::BeginFrame(const InputManager &inputManager, const CursorManager &cursorManager)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = { framebufferSize.x, framebufferSize.y };

        io.AddKeyEvent(ImGuiKey_Tab,                inputManager.IsKeyHeld(Key::Tab));
        io.AddKeyEvent(ImGuiKey_LeftArrow,          inputManager.IsKeyHeld(Key::LeftArrow));
        io.AddKeyEvent(ImGuiKey_RightArrow,         inputManager.IsKeyHeld(Key::RightArrow));
        io.AddKeyEvent(ImGuiKey_UpArrow,            inputManager.IsKeyHeld(Key::UpArrow));
        io.AddKeyEvent(ImGuiKey_DownArrow,          inputManager.IsKeyHeld(Key::DownArrow));
        io.AddKeyEvent(ImGuiKey_PageUp,             inputManager.IsKeyHeld(Key::PageUp));
        io.AddKeyEvent(ImGuiKey_PageDown,           inputManager.IsKeyHeld(Key::PageDown));
        io.AddKeyEvent(ImGuiKey_Home,               inputManager.IsKeyHeld(Key::Home));
        io.AddKeyEvent(ImGuiKey_End,                inputManager.IsKeyHeld(Key::End));
        io.AddKeyEvent(ImGuiKey_Insert,             inputManager.IsKeyHeld(Key::Insert));
        io.AddKeyEvent(ImGuiKey_Delete,             inputManager.IsKeyHeld(Key::Delete));
        io.AddKeyEvent(ImGuiKey_Backspace,          inputManager.IsKeyHeld(Key::Backspace));
        io.AddKeyEvent(ImGuiKey_Space,              inputManager.IsKeyHeld(Key::Space));
        io.AddKeyEvent(ImGuiKey_Enter,              inputManager.IsKeyHeld(Key::Enter));
        io.AddKeyEvent(ImGuiKey_Escape,             inputManager.IsKeyHeld(Key::Escape));
        io.AddKeyEvent(ImGuiKey_LeftCtrl,           inputManager.IsKeyHeld(Key::LeftControl));
        io.AddKeyEvent(ImGuiKey_LeftShift,          inputManager.IsKeyHeld(Key::LeftShift));
        io.AddKeyEvent(ImGuiKey_LeftAlt,            inputManager.IsKeyHeld(Key::LeftAlt));
        io.AddKeyEvent(ImGuiKey_LeftSuper,          inputManager.IsKeyHeld(Key::LeftSystem));
        io.AddKeyEvent(ImGuiKey_RightCtrl,          inputManager.IsKeyHeld(Key::RightControl));
        io.AddKeyEvent(ImGuiKey_RightShift,         inputManager.IsKeyHeld(Key::RightShift));
        io.AddKeyEvent(ImGuiKey_RightAlt,           inputManager.IsKeyHeld(Key::RightAlt));
        io.AddKeyEvent(ImGuiKey_RightSuper,         inputManager.IsKeyHeld(Key::RightSystem));
        io.AddKeyEvent(ImGuiKey_Menu,               inputManager.IsKeyHeld(Key::Menu));
        io.AddKeyEvent(ImGuiKey_0,                  inputManager.IsKeyHeld(Key::Number0));
        io.AddKeyEvent(ImGuiKey_1,                  inputManager.IsKeyHeld(Key::Number1));
        io.AddKeyEvent(ImGuiKey_2,                  inputManager.IsKeyHeld(Key::Number2));
        io.AddKeyEvent(ImGuiKey_3,                  inputManager.IsKeyHeld(Key::Number3));
        io.AddKeyEvent(ImGuiKey_4,                  inputManager.IsKeyHeld(Key::Number4));
        io.AddKeyEvent(ImGuiKey_5,                  inputManager.IsKeyHeld(Key::Number5));
        io.AddKeyEvent(ImGuiKey_6,                  inputManager.IsKeyHeld(Key::Number6));
        io.AddKeyEvent(ImGuiKey_7,                  inputManager.IsKeyHeld(Key::Number7));
        io.AddKeyEvent(ImGuiKey_8,                  inputManager.IsKeyHeld(Key::Number8));
        io.AddKeyEvent(ImGuiKey_9,                  inputManager.IsKeyHeld(Key::Number9));
        io.AddKeyEvent(ImGuiKey_A,                  inputManager.IsKeyHeld(Key::A));
        io.AddKeyEvent(ImGuiKey_B,                  inputManager.IsKeyHeld(Key::B));
        io.AddKeyEvent(ImGuiKey_C,                  inputManager.IsKeyHeld(Key::C));
        io.AddKeyEvent(ImGuiKey_D,                  inputManager.IsKeyHeld(Key::D));
        io.AddKeyEvent(ImGuiKey_E,                  inputManager.IsKeyHeld(Key::E));
        io.AddKeyEvent(ImGuiKey_F,                  inputManager.IsKeyHeld(Key::F));
        io.AddKeyEvent(ImGuiKey_G,                  inputManager.IsKeyHeld(Key::G));
        io.AddKeyEvent(ImGuiKey_H,                  inputManager.IsKeyHeld(Key::H));
        io.AddKeyEvent(ImGuiKey_I,                  inputManager.IsKeyHeld(Key::I));
        io.AddKeyEvent(ImGuiKey_J,                  inputManager.IsKeyHeld(Key::J));
        io.AddKeyEvent(ImGuiKey_K,                  inputManager.IsKeyHeld(Key::K));
        io.AddKeyEvent(ImGuiKey_L,                  inputManager.IsKeyHeld(Key::L));
        io.AddKeyEvent(ImGuiKey_M,                  inputManager.IsKeyHeld(Key::M));
        io.AddKeyEvent(ImGuiKey_N,                  inputManager.IsKeyHeld(Key::N));
        io.AddKeyEvent(ImGuiKey_O,                  inputManager.IsKeyHeld(Key::O));
        io.AddKeyEvent(ImGuiKey_P,                  inputManager.IsKeyHeld(Key::P));
        io.AddKeyEvent(ImGuiKey_Q,                  inputManager.IsKeyHeld(Key::Q));
        io.AddKeyEvent(ImGuiKey_R,                  inputManager.IsKeyHeld(Key::R));
        io.AddKeyEvent(ImGuiKey_S,                  inputManager.IsKeyHeld(Key::S));
        io.AddKeyEvent(ImGuiKey_T,                  inputManager.IsKeyHeld(Key::T));
        io.AddKeyEvent(ImGuiKey_U,                  inputManager.IsKeyHeld(Key::U));
        io.AddKeyEvent(ImGuiKey_V,                  inputManager.IsKeyHeld(Key::V));
        io.AddKeyEvent(ImGuiKey_W,                  inputManager.IsKeyHeld(Key::W));
        io.AddKeyEvent(ImGuiKey_X,                  inputManager.IsKeyHeld(Key::X));
        io.AddKeyEvent(ImGuiKey_Y,                  inputManager.IsKeyHeld(Key::Y));
        io.AddKeyEvent(ImGuiKey_Z,                  inputManager.IsKeyHeld(Key::Z));
        io.AddKeyEvent(ImGuiKey_F1,                 inputManager.IsKeyHeld(Key::F1));
        io.AddKeyEvent(ImGuiKey_F2,                 inputManager.IsKeyHeld(Key::F2));
        io.AddKeyEvent(ImGuiKey_F3,                 inputManager.IsKeyHeld(Key::F3));
        io.AddKeyEvent(ImGuiKey_F4,                 inputManager.IsKeyHeld(Key::F4));
        io.AddKeyEvent(ImGuiKey_F5,                 inputManager.IsKeyHeld(Key::F5));
        io.AddKeyEvent(ImGuiKey_F6,                 inputManager.IsKeyHeld(Key::F6));
        io.AddKeyEvent(ImGuiKey_F7,                 inputManager.IsKeyHeld(Key::F7));
        io.AddKeyEvent(ImGuiKey_F8,                 inputManager.IsKeyHeld(Key::F8));
        io.AddKeyEvent(ImGuiKey_F9,                 inputManager.IsKeyHeld(Key::F9));
        io.AddKeyEvent(ImGuiKey_F10,                inputManager.IsKeyHeld(Key::F10));
        io.AddKeyEvent(ImGuiKey_F11,                inputManager.IsKeyHeld(Key::F11));
        io.AddKeyEvent(ImGuiKey_F12,                inputManager.IsKeyHeld(Key::F12));
        io.AddKeyEvent(ImGuiKey_F13,                inputManager.IsKeyHeld(Key::F13));
        io.AddKeyEvent(ImGuiKey_F14,                inputManager.IsKeyHeld(Key::F14));
        io.AddKeyEvent(ImGuiKey_F15,                inputManager.IsKeyHeld(Key::F15));
        io.AddKeyEvent(ImGuiKey_F16,                inputManager.IsKeyHeld(Key::F16));
        io.AddKeyEvent(ImGuiKey_F17,                inputManager.IsKeyHeld(Key::F17));
        io.AddKeyEvent(ImGuiKey_F18,                inputManager.IsKeyHeld(Key::F18));
        io.AddKeyEvent(ImGuiKey_F19,                inputManager.IsKeyHeld(Key::F19));
        io.AddKeyEvent(ImGuiKey_F20,                inputManager.IsKeyHeld(Key::F20));
        io.AddKeyEvent(ImGuiKey_F21,                inputManager.IsKeyHeld(Key::F21));
        io.AddKeyEvent(ImGuiKey_F22,                inputManager.IsKeyHeld(Key::F22));
        io.AddKeyEvent(ImGuiKey_F23,                inputManager.IsKeyHeld(Key::F23));
        io.AddKeyEvent(ImGuiKey_F24,                inputManager.IsKeyHeld(Key::F24));
        io.AddKeyEvent(ImGuiKey_Apostrophe,         inputManager.IsKeyHeld(Key::Apostrophe));
        io.AddKeyEvent(ImGuiKey_Comma,              inputManager.IsKeyHeld(Key::Comma));
        io.AddKeyEvent(ImGuiKey_Minus,              inputManager.IsKeyHeld(Key::Minus));
        io.AddKeyEvent(ImGuiKey_Period,             inputManager.IsKeyHeld(Key::Period));
        io.AddKeyEvent(ImGuiKey_Slash,              inputManager.IsKeyHeld(Key::Slash));
        io.AddKeyEvent(ImGuiKey_Semicolon,          inputManager.IsKeyHeld(Key::Semicolon));
        io.AddKeyEvent(ImGuiKey_Equal,              inputManager.IsKeyHeld(Key::Equals));
        io.AddKeyEvent(ImGuiKey_LeftBracket,        inputManager.IsKeyHeld(Key::LeftBracket));
        io.AddKeyEvent(ImGuiKey_Backslash,          inputManager.IsKeyHeld(Key::Backslash));
        io.AddKeyEvent(ImGuiKey_RightBracket,       inputManager.IsKeyHeld(Key::RightBracket));
        io.AddKeyEvent(ImGuiKey_GraveAccent,        inputManager.IsKeyHeld(Key::Grave));
        io.AddKeyEvent(ImGuiKey_CapsLock,           inputManager.IsKeyHeld(Key::CapsLock));
        io.AddKeyEvent(ImGuiKey_ScrollLock,         inputManager.IsKeyHeld(Key::ScrollLock));
        io.AddKeyEvent(ImGuiKey_NumLock,            inputManager.IsKeyHeld(Key::NumpadLock));
        io.AddKeyEvent(ImGuiKey_PrintScreen,        inputManager.IsKeyHeld(Key::PrintScreen));
        io.AddKeyEvent(ImGuiKey_Pause,              inputManager.IsKeyHeld(Key::Pause));
        io.AddKeyEvent(ImGuiKey_Keypad0,            inputManager.IsKeyHeld(Key::KeypadNumber0));
        io.AddKeyEvent(ImGuiKey_Keypad5,            inputManager.IsKeyHeld(Key::KeypadNumber5));
        io.AddKeyEvent(ImGuiKey_KeypadDecimal,      inputManager.IsKeyHeld(Key::KeypadDecimal));
        io.AddKeyEvent(ImGuiKey_KeypadDivide,       inputManager.IsKeyHeld(Key::KeypadDivide));
        io.AddKeyEvent(ImGuiKey_KeypadMultiply,     inputManager.IsKeyHeld(Key::KeypadMultiply));
        io.AddKeyEvent(ImGuiKey_KeypadSubtract,     inputManager.IsKeyHeld(Key::KeypadSubtract));
        io.AddKeyEvent(ImGuiKey_KeypadAdd,          inputManager.IsKeyHeld(Key::KeypadAdd));
        io.AddKeyEvent(ImGuiKey_KeypadEnter,        inputManager.IsKeyHeld(Key::KeypadEnter));
        io.AddKeyEvent(ImGuiKey_KeypadEqual,        inputManager.IsKeyHeld(Key::KeypadEquals));
        io.AddKeyEvent(ImGuiKey_AppBack,            false);
        io.AddKeyEvent(ImGuiKey_AppForward,         false);

        io.KeyCtrl      = inputManager.IsKeyHeld(Key::LeftControl) || inputManager.IsKeyHeld(Key::RightControl);
        io.KeyShift     = inputManager.IsKeyHeld(Key::LeftShift)   || inputManager.IsKeyHeld(Key::RightShift);
        io.KeyAlt       = inputManager.IsKeyHeld(Key::LeftAlt)     || inputManager.IsKeyHeld(Key::RightAlt);
        io.KeySuper     = inputManager.IsKeyHeld(Key::LeftSystem)  || inputManager.IsKeyHeld(Key::RightSystem);

        if (inputManager.IsKeyPressed(Key::A)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::A) : std::toupper(GetKeyCharacter(Key::A))); }
        if (inputManager.IsKeyPressed(Key::B)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::B) : std::toupper(GetKeyCharacter(Key::B))); }
        if (inputManager.IsKeyPressed(Key::C)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::C) : std::toupper(GetKeyCharacter(Key::C))); }
        if (inputManager.IsKeyPressed(Key::D)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::D) : std::toupper(GetKeyCharacter(Key::D))); }
        if (inputManager.IsKeyPressed(Key::E)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::E) : std::toupper(GetKeyCharacter(Key::E))); }
        if (inputManager.IsKeyPressed(Key::F)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::F) : std::toupper(GetKeyCharacter(Key::F))); }
        if (inputManager.IsKeyPressed(Key::G)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::G) : std::toupper(GetKeyCharacter(Key::G))); }
        if (inputManager.IsKeyPressed(Key::H)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::H) : std::toupper(GetKeyCharacter(Key::H))); }
        if (inputManager.IsKeyPressed(Key::I)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::I) : std::toupper(GetKeyCharacter(Key::I))); }
        if (inputManager.IsKeyPressed(Key::J)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::J) : std::toupper(GetKeyCharacter(Key::J))); }
        if (inputManager.IsKeyPressed(Key::K)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::K) : std::toupper(GetKeyCharacter(Key::K))); }
        if (inputManager.IsKeyPressed(Key::L)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::L) : std::toupper(GetKeyCharacter(Key::L))); }
        if (inputManager.IsKeyPressed(Key::M)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::M) : std::toupper(GetKeyCharacter(Key::M))); }
        if (inputManager.IsKeyPressed(Key::N)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::N) : std::toupper(GetKeyCharacter(Key::N))); }
        if (inputManager.IsKeyPressed(Key::O)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::O) : std::toupper(GetKeyCharacter(Key::O))); }
        if (inputManager.IsKeyPressed(Key::P)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::P) : std::toupper(GetKeyCharacter(Key::P))); }
        if (inputManager.IsKeyPressed(Key::Q)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Q) : std::toupper(GetKeyCharacter(Key::Q))); }
        if (inputManager.IsKeyPressed(Key::R)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::R) : std::toupper(GetKeyCharacter(Key::R))); }
        if (inputManager.IsKeyPressed(Key::S)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::S) : std::toupper(GetKeyCharacter(Key::S))); }
        if (inputManager.IsKeyPressed(Key::T)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::T) : std::toupper(GetKeyCharacter(Key::T))); }
        if (inputManager.IsKeyPressed(Key::U)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::U) : std::toupper(GetKeyCharacter(Key::U))); }
        if (inputManager.IsKeyPressed(Key::V)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::V) : std::toupper(GetKeyCharacter(Key::V))); }
        if (inputManager.IsKeyPressed(Key::W)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::W) : std::toupper(GetKeyCharacter(Key::W))); }
        if (inputManager.IsKeyPressed(Key::X)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::X) : std::toupper(GetKeyCharacter(Key::X))); }
        if (inputManager.IsKeyPressed(Key::Y)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Y) : std::toupper(GetKeyCharacter(Key::Y))); }
        if (inputManager.IsKeyPressed(Key::Z)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Z) : std::toupper(GetKeyCharacter(Key::Z))); }
        if (inputManager.IsKeyPressed(Key::Space)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Space) : std::toupper(GetKeyCharacter(Key::Space))); }

        io.MouseDown[ImGuiMouseButton_Left]         = inputManager.IsMouseButtonHeld(MouseButton::Left);
        io.MouseDown[ImGuiMouseButton_Right]        = inputManager.IsMouseButtonHeld(MouseButton::Right);
        io.MouseDown[ImGuiMouseButton_Middle]       = inputManager.IsMouseButtonHeld(MouseButton::Middle);
        io.MouseReleased[ImGuiMouseButton_Left]     = inputManager.IsMouseButtonReleased(MouseButton::Left);
        io.MouseReleased[ImGuiMouseButton_Right]    = inputManager.IsMouseButtonReleased(MouseButton::Right);
        io.MouseReleased[ImGuiMouseButton_Middle]   = inputManager.IsMouseButtonReleased(MouseButton::Middle);
        io.MouseWheel                               = inputManager.GetMouseScroll().y / 10.0f;
        io.MouseWheelH                              = inputManager.GetMouseScroll().x / 10.0f;

        io.MousePos                                 = !cursorManager.IsCursorHidden() ? ImVec2(cursorManager.GetCursorPosition().x, framebufferSize.y - cursorManager.GetCursorPosition().y) : ImVec2(std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min());
        io.MouseDelta                               = { cursorManager.GetHorizontalDelta(), -cursorManager.GetVerticalDelta() };

        ImGui::GetStyle() = *reinterpret_cast<::ImGuiStyle*>(style);
    }

    void ImGuiRenderTask::EndFrame()
    {
        ImGui::ShowDemoWindow();
        ImGui::Render();
    }

    void ImGuiRenderTask::Resize(const uint32 width, const uint32 height)
    {
        framebufferSize = { width, height };
    }

    /* --- DESTRUCTOR --- */

    ImGuiRenderTask::~ImGuiRenderTask()
    {
        activeTaskCount--;
        delete(reinterpret_cast<::ImGuiStyle*>(style));

        if (activeTaskCount == 0)
        {
            ImGui::DestroyContext(reinterpret_cast<ImGuiContext*>(context));
            context = nullptr;
        }
    }

}