//
// Created by Nikolay Kanchevski on 20.07.23.
//

#pragma once

#include "GUITypes.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Handlers/Assets/ResourceSystem.h"

namespace Sierra::Editor::GUI
{
    /* --- WIDGETS --- */
    void Text(const char* text, ...);
    void ColoredText(Vector4 color, const char* text, ...);
    bool StringInput(const char* label, String &value, InputFlags inputFlags = InputFlags::NONE);
    bool Int32Input(const char* label, int32 &value, bool canDrag = false, InputFlags inputFlags = InputFlags::NONE);
    bool Int64Input(const char* label, int64 &value, bool canDrag = false, InputFlags inputFlags = InputFlags::NONE);
    bool UInt32Input(const char* label, uint32 &value, bool canDrag = false, InputFlags inputFlags = InputFlags::NONE);
    bool UInt64Input(const char* label, uint64 &value, bool canDrag = false, InputFlags inputFlags = InputFlags::NONE);
    bool FloatInput(const char* label, float &value, bool canDrag = false, InputFlags inputFlags = InputFlags::NONE);
    bool DoubleInput(const char* label, double &value, bool canDrag = false, InputFlags inputFlags = InputFlags::NONE);
    bool Checkbox(const char* label, bool &value);
    bool Checkbox(const char* label, uint32 &value);
    bool RadioButton(const char* label, bool active);
    bool Vector3Input(Vector3 &value, const float* resetValues);
    bool Dropdown(const char* label, uint32 &value, const char* *options, uint32 optionCount, const bool* deactivatedFlags = nullptr);
    void Texture(ImTextureID textureID, const Vector2 &size, const Vector2 &UV0 = { 0.0f, 0.0f }, const Vector2 &UV1 = { 1.0f, 1.0f }, const Vector4 &tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vector4 &borderColor = { 0.0f, 0.0f, 0.0f, 0.0f });
    bool Button(const char* label);
    bool Button(const char* label, const Vector2 &size);
    bool RoundedButton(const char* label, const Vector2 &size, CornerRoundingFlags roundingFlags = CornerRoundingFlags::DEFAULT, float rounding = GImGui->Style.FrameRounding);
    void LineDiagram(const char* label, const float* values, uint32 valueCount, uint32 currentValueIndex, const char* text = nullptr, float minimumValue = FLOAT_MIN, float maximumValue = FLOAT_MAX, const Vector2 &size = { 0.0f, 0.0f });
    bool MenuItem(const char* text);
    void OpenPopup(const char* text);

    /* --- POLLING METHODS --- */
    void CreateDockSpace(const char* ID);

    bool BeginMenuBar();
    void EndMenuBar();

    bool BeginMenuBarTab(const char* tabName);
    void EndMenuBarTab();

    bool BeginWindow(const char* title = "Debug", WindowFlags windowFlags = WindowFlags::NONE);
    void EndWindow();

    bool BeginTree(const void* pointerID, TreeFlags treeFlags, const char* text, ...);
    bool BeginTree(const char* text, TreeFlags treeFlags = TreeFlags::NONE);
    void EndTree();

    bool BeginPopup(const char* text);
    void EndPopup();

    void PushDeactivatedStatus();
    void PopDeactivatedStatus();

    void PushStyleColor(StyleColor styleColor, const Vector4 &color);
    void PopStyleColor(uint32 count = 1);

    void PushStyleVariable(StyleVariable styleVariable, float value);
    void PushStyleVariable(StyleVariable styleVariable, const Vector2 &value);
    void PopStyleVariable(uint32 count = 1);

    void PushFont(ImFont* font);
    void PopFont();

    void PushFontSize(float size);
    void PopFontSize();

    void PushDefaultFont();
    void PushBoldFont();

    void InsertSeparator();
    void ContinueOnSameLine(float indent = 0.0f);
    void HorizontalIndent(const float space = 2.0f);
    void HorizontalUnindent(const float space = 2.0f);
    void VerticalIndent(const float space = 2.0f);

    /* --- SETTER METHODS --- */
    void SetCurrentPosition(const Vector2 &position);
    void SetNextWindowPosition(const Vector2 &position);
    void SetNextWindowSize(const Vector2 &size);
    void SetNextWindowSizeConstraints(const Vector2 &minimumSize, const Vector2 &maximumSize);
    void SetNextWindowCollapsed(bool collapse);

    void SetNextItemWidth(float width);
    void SetNextItemWidthToFit();

    void SetNumericInputFormattingSpaceCount(uint32 spaceCount);
    void ResetNumericInputFormattingSpaceCount();

    void SetNumericInputStepChange(float newStepChange);
    void ResetNumericInputStepChange();

    void SetNumericInputDragSpeed(float newDragSpeed);
    void ResetNumericInputDragSpeed();

    void SetNumericInputLimits(Optional<float> minimum, Optional<float> maximum);
    void ResetNumericInputLimits();

    /* --- GETTER METHODS --- */
    [[nodiscard]] uint32 GetID(const char* string);
    [[nodiscard]] uint32 HashString(const char* string);
    [[nodiscard]] ImGuiViewport* GetMainViewport();

    [[nodiscard]] ImGuiWindow* GetCurrentWindow();
    [[nodiscard]] ImGuiWindow* GetHoveredWindow();

    [[nodiscard]] float GetFontSize();

    [[nodiscard]] Vector2 GetRemainingWindowSpace();
    [[nodiscard]] float GetRemainingHorizontalSpace();
    [[nodiscard]] float GetRemainingVerticalSpace();

    [[nodiscard]] bool IsItemClicked();
    [[nodiscard]] bool IsItemDoubleClicked();
    [[nodiscard]] bool IsWindowHovered();
    [[nodiscard]] bool IsItemHovered();
    [[nodiscard]] ImDrawData* GetDrawData();

    /* --- EDITOR WIDGETS --- */
    void BeginProperties();
    void EndProperties();

    void BeginProperty(const char* label, const char* tooltip = nullptr);
    void EndProperty();

    bool BeginTreeProperty(const char* label);
    void EndTreeProperty();

    /* --- EDITOR PROPERTY WIDGETS --- */
    bool StringProperty(const char* label, String &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool Int32Property(const char* label, int32 &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool Int64Property(const char* label, int64 &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool UInt32Property(const char* label, uint32 &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool UInt64Property(const char* label, uint64 &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool FloatProperty(const char* label, float &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool DoubleProperty(const char* label, double &value, const char* tooltip = nullptr, InputFlags inputFlags = InputFlags::NONE);
    bool CheckboxProperty(const char* label, bool &value, const char* tooltip = nullptr);
    bool CheckboxProperty(const char* label, uint32 &value, const char* tooltip = nullptr);
    bool DropdownProperty(const char* label, uint32 &value, const char* *options, uint32 optionCount, const bool* deactivatedFlags = nullptr, const char* tooltip = nullptr);
    bool Vector3Property(const char* label, Vector3 &value, const char* tooltip = nullptr, const float* resetValues = nullptr);
    bool TextureProperty(const char* label, SharedPtr<Rendering::Texture> &texture, Engine::TextureType textureType, const char* tooltip = nullptr);
    template<typename T>
    inline consteval bool AnyPropertyInput(const char* label, T &value)
    {
        if constexpr(std::is_same_v<T, String>) { return StringProperty(label, value); }
        else if constexpr(std::is_same_v<T, int32>) { return Int32Property(label, value); }
        else if constexpr(std::is_same_v<T, int64>) { return Int64Property(label, value); }
        else if constexpr(std::is_same_v<T, uint32>) { return UInt32Property(label, value); }
        else if constexpr(std::is_same_v<T, uint64>) { return UInt64Property(label, value); }
        else if constexpr(std::is_same_v<T, float>) { return FloatProperty(label, value); }
        else if constexpr(std::is_same_v<T, double>) { return DoubleProperty(label, value); }
        else if constexpr(std::is_same_v<T, bool>) { return CheckboxProperty(label, value); }
        else if constexpr(std::is_same_v<T, Vector3>) { const float resetValues[3] = { 0.0f, 0.0f, 0.0f }; return Vector3Property(label, value, resetValues); }
        else if constexpr(std::is_same_v<T, SharedPtr<Rendering::Texture>>) { return TextureProperty(label, value); }
        else return false;
    }

    template<typename T>
    inline void DrawComponent(const Engine::Entity &entity)
    {
        const TreeFlags treeFlags = TreeFlags::OPEN_BY_DEFAULT | TreeFlags::FRAMED | TreeFlags::SPAN_AVAILABLE_WIDTH | TreeFlags::ALLOW_ITEM_OVERLAP | TreeFlags::FRAME_PADDING;
        if (entity.HasComponent<T>())
        {
            auto &component = entity.GetComponent<T>();
            GUI::PushStyleVariable(StyleVariable::FRAME_PADDING, { 4.0f, 4.0f });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

            bool open = GUI::BeginTree((void*)(typeid(T).hash_code()), treeFlags, "%s", Debugger::TypeToString<T>().c_str());
            GUI::PopStyleVariable();

            GUI::ContinueOnSameLine(GUI::GetRemainingHorizontalSpace() + 10.0f);
            if (GUI::Button("+", { lineHeight, lineHeight }))
            {
                GUI::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (GUI::BeginPopup("ComponentSettings"))
            {
                if (GUI::MenuItem("Remove component"))
                {
                    removeComponent = true;
                }

                GUI::EndPopup();
            }

            if (open)
            {
                component.OnDrawUI();
                GUI::VerticalIndent(GImGui->Style.ItemSpacing.y);
                GUI::EndTree();
            }

            if (removeComponent)
            {
                // Engine::World::RemoveComponent<T>(entity);
            }
        }
    }
}
