//
// Created by Nikolay Kanchevski on 20.07.23.
//

#include "GUI.h"

#include "../Engine/Handlers/Assets/AssetManager.h"

#define TO_IM_VEC2(VECTOR) reinterpret_cast<const ImVec2&>(VECTOR)
#define TO_IM_VEC4(VECTOR) reinterpret_cast<const ImVec4&>(VECTOR)

namespace Sierra::Editor
{
    /* --- PROPERTIES --- */

    char IDBuffer[16];
    int32 counter = 0;
    int32 contextID = 0;

    float oldFontSize = 0;
    constexpr float HOVER_TIME_THRESHOLD = 1.0f;

    bool setNextItemDisabled = false;
    String numericInputFormatting = ".2";

    float numericInputStepChange = 1.0f;
    float numericInputDragSpeed = 0.1f;

    float numericInputLimitMin = 0.0f;
    float numericInputLimitMax = 0.0f;


    /* --- CORE FUNCTIONALITIES --- */

    void GenerateID()
    {
        IDBuffer[0] = '#';
        IDBuffer[1] = '#';
        memset(IDBuffer + 2, 0, 14);
        ++counter;
        String buffer = FORMAT_STRING("##{0}", counter);
        std::memcpy(&IDBuffer, buffer.data(), 16);
    }

    void PushID()
    {
        ++contextID;
        ImGui::PushID(contextID);
        counter = 0;
    }

    void PopID()
    {
        ImGui::PopID();
        --contextID;
    }

    /* --- UTILITIES --- */

    template<typename T, ENABLE_IF(std::is_arithmetic_v<T>)>
    static consteval inline ImGuiDataType GetNumericImGuiDataType()
    {
        if (std::is_same<T, int8>()) return ImGuiDataType_S8;
        if (std::is_same<T, uint8>()) return ImGuiDataType_U8;
        if (std::is_same<T, int16>()) return ImGuiDataType_S16;
        if (std::is_same<T, uint16>()) return ImGuiDataType_U16;
        if (std::is_same<T, int32>()) return ImGuiDataType_S32;
        if (std::is_same<T, uint32>()) return ImGuiDataType_U32;
        if (std::is_same<T, int64>()) return ImGuiDataType_S64;
        if (std::is_same<T, uint64>()) return ImGuiDataType_U64;
        if (std::is_same<T, float>()) return ImGuiDataType_Float;
        if (std::is_same<T, double>()) return ImGuiDataType_Double;
        return -1;
    }

    template<typename T, ENABLE_IF(std::is_arithmetic_v<T>)>
    static inline String GetSuitableNumericFormatting()
    {
        auto dataType = GetNumericImGuiDataType<T>();
        String dataTypeFormatting = String(ImGui::DataTypeGetInfo(dataType)->ScanFmt);
        dataTypeFormatting = dataTypeFormatting.insert(1, numericInputFormatting);
        return dataTypeFormatting;
    }

    /* --- WIDGETS --- */

    void GUI::Text(const char* text, ...)
    {
        va_list args;
        va_start(args, text);
        ImGui::TextV(text, args);
        va_end(args);
    }

    void GUI::ColoredText(Vector4 color, const char* text, ...)
    {
        va_list args;
        va_start(args, text);
        ImGui::TextColoredV({ color.r, color.g, color.b, color.a }, text, args);
        va_end(args);
    }
    
    bool GUI::StringInput(const char* label, String &value, const InputFlags inputFlags)
    {
        return ImGui::InputText(label, &value, static_cast<ImGuiInputTextFlags>(inputFlags), nullptr);
    }

    // This ugly code is also mostly copied from the ImGui's source (combines ImGui's core functionality for integer input and float input inside this one function, see: see: https://github.com/ocornut/imgui/blob/master/imgui_widgets.cpp)
    template<typename T, ENABLE_IF(std::is_arithmetic_v<T>)>
    static inline bool NumericInput(const char* label, T &valueReference, bool canDrag, bool centerAlign, InputFlags flags = InputFlags::NONE, ImDrawFlags roundingType = ImDrawFlags_RoundCornersAll, float rounding = GImGui->Style.FrameRounding)
    {
        ImGuiDataType dataType = GetNumericImGuiDataType<T>();
        auto value = &valueReference;

        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext &g = *GImGui;
        ImGuiStyle &style = g.Style;

        String formatString = GetSuitableNumericFormatting<T>();
        const char* localFormat = formatString.c_str();

        if (!canDrag)
        {
            char buffer[64];
            ImGui::DataTypeFormatString(buffer, IM_ARRAYSIZE(buffer), dataType, value, localFormat);
            flags |= InputFlags::AUTO_SELECT_ALL | InputFlags::NO_MARK_EDITED;

            bool valueChanged = false;
            if (numericInputStepChange != static_cast<T>(0))
            {
                const float buttonSize = ImGui::GetFrameHeight();

                ImGui::BeginGroup();
                ImGui::PushID(label);
                ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::CalcItemWidth() - (buttonSize + style.ItemInnerSpacing.x) * 2));
                if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), static_cast<ImGuiInputTextFlags>(flags))) valueChanged = ImGui::DataTypeApplyFromText(buffer, dataType, value, localFormat);
                IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags);

                const ImVec2 backupFramePadding = style.FramePadding;
                style.FramePadding.x = style.FramePadding.y;
                ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;

                if (flags & InputFlags::READ_ONLY) ImGui::BeginDisabled();
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (ImGui::ButtonEx("-", ImVec2(buttonSize, buttonSize), buttonFlags))
                {
                    valueReference -= g.IO.KeyCtrl ? static_cast<T>(numericInputStepChange) * 1.6f : static_cast<T>(numericInputStepChange);
                    valueChanged = true;
                }
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (ImGui::ButtonEx("+", ImVec2(buttonSize, buttonSize), buttonFlags))
                {
                    valueReference += g.IO.KeyCtrl ? static_cast<T>(numericInputStepChange) * 1.6f : static_cast<T>(numericInputStepChange);
                    valueChanged = true;
                }
                if (flags & InputFlags::READ_ONLY) ImGui::EndDisabled();

                const char* labelEnd = ImGui::FindRenderedTextEnd(label);
                if (label != labelEnd)
                {
                    ImGui::SameLine(0, style.ItemInnerSpacing.x);
                    ImGui::TextEx(label, labelEnd);
                }
                style.FramePadding = backupFramePadding;

                ImGui::PopID();
                ImGui::EndGroup();
            }
            else
            {
                if (ImGui::InputText(label, buffer, IM_ARRAYSIZE(buffer), static_cast<ImGuiInputTextFlags>(flags))) valueChanged = ImGui::DataTypeApplyFromText(buffer, dataType, value, localFormat);
            }

            if (valueChanged) ImGui::MarkItemEdited(g.LastItemData.ID);
            return valueChanged;
        }
        else
        {
            const ImGuiID ID = window->GetID(label);
            const float width = ImGui::CalcItemWidth();

            const ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);
            const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, labelSize.y + style.FramePadding.y * 2.0f));
            const ImRect totalBB(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.0f ? style.ItemInnerSpacing.x + labelSize.x : 0.0f, 0.0f));

            ImGui::ItemSize(totalBB, style.FramePadding.y);
            if (!ImGui::ItemAdd(totalBB, ID, &frameBB, ImGuiItemFlags_Inputable))
                return false;

            const bool hovered = ImGui::ItemHoverable(frameBB, ID);
            bool inputIsActive = ImGui::TempInputIsActive(ID);
            if (!inputIsActive)
            {
                // Tabbing or CTRL-clicking on Drag turns it into an InputText
                const bool input_requested_by_tabbing =
                        (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
                const bool clicked = (hovered && g.IO.MouseClicked[0]);
                const bool doubleClicked = (hovered && g.IO.MouseClickedCount[0] == 2);
                const bool makeActive = (input_requested_by_tabbing || clicked || doubleClicked ||
                                          g.NavActivateId == ID);
                if (makeActive)
                {
                    if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || doubleClicked || (g.NavActivateId == ID && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput))) inputIsActive = true;
                }

                if (g.IO.ConfigDragClickToInputText && !inputIsActive)
                {
                    if (g.ActiveId == ID && hovered && g.IO.MouseReleased[0] && !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * 0.50f)) // DRAG_MOUSE_THRESHOLD_FACTOR
                    {
                        g.NavActivateId = ID;
                        g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
                        inputIsActive = true;
                    }
                }

                if (makeActive && !inputIsActive)
                {
                    ImGui::SetActiveID(ID, window);
                    ImGui::SetFocusID(ID, window);
                    ImGui::FocusWindow(window);
                    g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
                }
            }

            if (inputIsActive)
            {
                const bool clampInput = ImGui::DataTypeCompare(dataType, &numericInputLimitMin, &numericInputLimitMax) < 0;
                return ImGui::TempInputScalar(frameBB, ID, label, dataType, value, localFormat, clampInput ? &numericInputLimitMin : nullptr, clampInput ? &numericInputLimitMax : nullptr);
            }

            const ImU32 frameColor = ImGui::GetColorU32(g.ActiveId == ID ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
            ImGui::RenderNavHighlight(frameBB, ID);
            ImGui::GetWindowDrawList()->AddRectFilled(frameBB.Min, frameBB.Max, frameColor, rounding, roundingType);

            const bool valueChanged = ImGui::DragBehavior(ID, dataType, value, numericInputDragSpeed, &numericInputLimitMin, &numericInputLimitMax, localFormat, 0);
            if (valueChanged) ImGui::MarkItemEdited(ID);

            // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
            char valueBuffer[64];
            const char* valueBufferEnd = valueBuffer + ImGui::DataTypeFormatString(valueBuffer, IM_ARRAYSIZE(valueBuffer), dataType, value, localFormat);
            if (g.LogEnabled) ImGui::LogSetNextTextDecoration("{", "}");

            float xAlign = centerAlign ? 0.5f : g.Style.ItemInnerSpacing.x / ImGui::GetItemRectSize().x;
            float yAlign = centerAlign ? 0.5f : 0.0f;
            ImGui::RenderTextClipped(frameBB.Min, frameBB.Max, valueBuffer, valueBufferEnd, nullptr, ImVec2(xAlign, yAlign));

            if (labelSize.x > 0.0f) ImGui::RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, frameBB.Min.y + style.FramePadding.y), label);

            IMGUI_TEST_ENGINE_ITEM_INFO(ID, label, g.LastItemData.StatusFlags);
            return valueChanged;
        }
    }

    bool GUI::Int32Input(const char* label, int32 &value, bool canDrag, InputFlags inputFlags)
    {
        return NumericInput<int32>(label, value, canDrag, false, inputFlags);
    }

    bool GUI::Int64Input(const char* label, int64 &value, bool canDrag, const InputFlags inputFlags)
    {
        return NumericInput<int64>(label, value, canDrag, false, inputFlags);
    }

    bool GUI::UInt32Input(const char* label, uint32 &value, bool canDrag, InputFlags inputFlags)
    {
        return NumericInput<uint32>(label, value, canDrag, false, inputFlags);
    }

    bool GUI::UInt64Input(const char* label, uint64 &value, bool canDrag, const InputFlags inputFlags)
    {
        return NumericInput<uint64>(label, value, canDrag, false, inputFlags);
    }

    bool GUI::FloatInput(const char* label, float &value, bool canDrag, const InputFlags inputFlags)
    {
        return NumericInput<float>(label, value, canDrag, false, inputFlags);
    }

    bool GUI::DoubleInput(const char* label, double &value, bool canDrag, const InputFlags inputFlags)
    {
        return NumericInput<double>(label, value, canDrag, false, inputFlags);
    }

    bool GUI::Checkbox(const char* label, bool &value)
    {
        return ImGui::Checkbox(label, &value);
    }

    bool GUI::Checkbox(const char* label, uint32 &value)
    {
        bool boolValue;
        bool modified = ImGui::Checkbox(label, &boolValue);

        value = boolValue ? 1 : 0;
        return modified;
    }

    bool GUI::RadioButton(const char* label, bool active)
    {
        return ImGui::RadioButton(label, active);
    }

    bool GUI::Vector3Input(Vector3 &value, const float* resetValues)
    {
        float frameHeight = ImGui::GetFrameHeight();
        Vector2 buttonSize = { frameHeight, frameHeight };

        bool drawButtons = true;
        float inputFieldWidth = (ImGui::GetColumnWidth(1) - 2.0f * ImGui::GetStyle().ItemSpacing.x) / 3.0f - buttonSize.x;

        ImDrawFlags inputFieldRounding = ImDrawFlags_RoundCornersRight;
        if (buttonSize.x > 0.45f * inputFieldWidth)
        {
            drawButtons = false;
            inputFieldWidth += buttonSize.x;
            inputFieldRounding = ImDrawFlags_RoundCornersAll;
        }

        bool modified = false;

        // X
        {
            if (drawButtons)
            {
                GUI::PushBoldFont();
                GUI::PushStyleVariable(StyleVariable::ITEM_SPACING, { 0.00f, 0.00f });
                GUI::PushStyleColor(StyleColor::TEXT,               { 1.00f, 1.00f, 1.00f, 1.00f });
                GUI::PushStyleColor(StyleColor::BUTTON,             { 0.80f, 0.10f, 0.15f, 1.00f });
                GUI::PushStyleColor(StyleColor::HOVERED_BUTTON,     { 0.90f, 0.20f, 0.20f, 1.00f });
                GUI::PushStyleColor(StyleColor::ACTIVE_BUTTON,      { 0.80f, 0.10f, 0.15f, 1.00f });

                if (GUI::RoundedButton("X", buttonSize, CornerRoundingFlags::LEFT))
                {
                    value.x = resetValues != nullptr ? resetValues[0] : 0.0f;
                    modified = true;
                }

                GUI::PopFont();
                GUI::PopStyleColor(4);

                GUI::ContinueOnSameLine(0);
            }

            GUI::SetNextItemWidth(inputFieldWidth);
            if (NumericInput<float>("##X", value.x, true, true, InputFlags::NONE, inputFieldRounding))
            {
                modified = true;
            }

            if (drawButtons) GUI::PopStyleVariable();
        }

        GUI::ContinueOnSameLine(0);

        // Y
        {
            if (drawButtons)
            {
                GUI::PushBoldFont();
                GUI::PushStyleVariable(StyleVariable::ITEM_SPACING, { 0.00f, 0.00f });
                GUI::PushStyleColor(StyleColor::TEXT,               { 1.00f, 1.00f, 1.00f, 1.00f });
                GUI::PushStyleColor(StyleColor::BUTTON,             { 0.20f, 0.70f, 0.20f, 1.00f });
                GUI::PushStyleColor(StyleColor::HOVERED_BUTTON,     { 0.30f, 0.80f, 0.30f, 1.00f });
                GUI::PushStyleColor(StyleColor::ACTIVE_BUTTON,      { 0.20f, 0.70f, 0.20f, 1.00f });

                if (GUI::RoundedButton("Y", buttonSize, CornerRoundingFlags::LEFT))
                {
                    value.y = resetValues != nullptr ? resetValues[1] : 0.0f;
                    modified = true;
                }

                GUI::PopFont();
                GUI::PopStyleColor(4);

                GUI::ContinueOnSameLine(0);
            }

            GUI::SetNextItemWidth(inputFieldWidth);
            if (NumericInput<float>("##Y", value.y, true, true, InputFlags::NONE, inputFieldRounding))
            {
                modified = true;
            }

            if (drawButtons) GUI::PopStyleVariable();
        }

        GUI::ContinueOnSameLine(0);

        // Z
        {
            if (drawButtons)
            {
                GUI::PushBoldFont();
                GUI::PushStyleVariable(StyleVariable::ITEM_SPACING, { 0.00f, 0.00f });
                GUI::PushStyleColor(StyleColor::TEXT,               { 1.00f, 1.00f, 1.00f, 1.00f });
                GUI::PushStyleColor(StyleColor::BUTTON,             { 0.10f, 0.25f, 0.80f, 1.00f });
                GUI::PushStyleColor(StyleColor::HOVERED_BUTTON,     { 0.20f, 0.35f, 0.90f, 1.00f });
                GUI::PushStyleColor(StyleColor::ACTIVE_BUTTON,      { 0.10f, 0.25f, 0.80f, 1.00f });

                if (GUI::RoundedButton("Z", buttonSize, CornerRoundingFlags::LEFT))
                {
                    value.z = resetValues != nullptr ? resetValues[2] : 0.0f;
                    modified = true;
                }

                GUI::PopFont();
                GUI::PopStyleColor(4);

                GUI::ContinueOnSameLine(0);
            }

            GUI::SetNextItemWidth(inputFieldWidth);
            if (NumericInput<float>("##Z", value.z, true, true, InputFlags::NONE, inputFieldRounding))
            {
                modified = true;
            }

            if (drawButtons) GUI::PopStyleVariable();
        }

        return modified;
    }

    bool GUI::Dropdown(const char* label, uint32 &value, const char* *options, const uint32 optionCount, const bool* deactivatedFlags)
    {
        bool modified = false;
        const char* currentOption = options[value];

        if (ImGui::BeginCombo(label, currentOption))
        {
            for (uint32 i = 0; i < optionCount; i++)
            {
                bool selected = (currentOption == options[i]);

                bool deactivated = false;
                if (deactivatedFlags != nullptr && deactivatedFlags[i])
                {
                    deactivated = true;
                    PushDeactivatedStatus();
                }

                if (ImGui::Selectable(options[i], selected) && currentOption != options[i])
                {
                    currentOption = options[i];
                    value = i;
                    modified = true;
                }

                if (selected) ImGui::SetItemDefaultFocus();
                if (deactivated) PopDeactivatedStatus();
            }

            ImGui::EndCombo();
        }

        return modified;
    }

    void GUI::Texture(const ImTextureID textureID, const Vector2 &size, const Vector2 &UV0, const Vector2 &UV1, const Vector4 &tintColor, const Vector4 &borderColor)
    {
        ImGui::Image(textureID, TO_IM_VEC2(size), TO_IM_VEC2(UV0), TO_IM_VEC2(UV1), TO_IM_VEC4(tintColor), TO_IM_VEC4(borderColor));
    }

    bool GUI::Button(const char* label)
    {
        return ImGui::Button(label);
    }

    bool GUI::Button(const char* label, const Vector2 &size)
    {
        return ImGui::Button(label, TO_IM_VEC2(size));
    }

    // The code below is a simplified & refactored version of ImGui's core button functionality (see: https://github.com/ocornut/imgui/blob/master/imgui_widgets.cpp).
    bool GUI::RoundedButton(const char* label, const Vector2 &size, const CornerRoundingFlags roundingFlags, const float rounding)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext &g = *GImGui;
        const ImGuiStyle &style = g.Style;
        const ImGuiID ID = window->GetID(label);
        const ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);

        ImVec2 position = window->DC.CursorPos;
        if (style.FramePadding.y < window->DC.CurrLineTextBaseOffset) position.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
        ImVec2 finalSize = ImGui::CalcItemSize(TO_IM_VEC2(size), labelSize.x + style.FramePadding.x * 2.0f, labelSize.y + style.FramePadding.y * 2.0f);

        const ImRect bb(position, position + finalSize);
        ImGui::ItemSize(finalSize, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, ID)) return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, ID, &hovered, &held, 0);

        const ImU32 color = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        ImGui::RenderNavHighlight(bb, ID);
        ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, color, rounding, static_cast<ImDrawFlags>(roundingFlags));
        if (g.LogEnabled) ImGui::LogSetNextTextDecoration("[", "]");

        ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, nullptr, &labelSize, style.ButtonTextAlign, &bb);
        return pressed;
    }
    
    void GUI::LineDiagram(const char* label, const float *values, const uint32 valueCount, const uint32 currentValueIndex, const char* text, const float minimumValue, const float maximumValue, const Vector2 &size)
    {
        ImGui::PlotLines(label, values, valueCount, currentValueIndex, text, minimumValue, maximumValue, TO_IM_VEC2(size));
    }
    
    bool GUI::MenuItem(const char* text)
    {
        return ImGui::MenuItem(text);
    }

    void GUI::OpenPopup(const char* text)
    {
        ImGui::OpenPopup(text);
    }

    /* --- POLLING METHODS --- */

    void GUI::CreateDockSpace(const char* ID)
    {
        ImGui::DockSpace(ImGui::GetID(ID), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    bool GUI::BeginMenuBar()
    {
        return ImGui::BeginMenuBar();
    }

    void GUI::EndMenuBar()
    {
        ImGui::EndMenuBar();
    }

    bool GUI::BeginMenuBarTab(const char* tabName)
    {
        return ImGui::BeginMenu(tabName);
    }

    void GUI::EndMenuBarTab()
    {
        ImGui::EndMenu();
    }


    bool GUI::BeginWindow(const char* title, const WindowFlags windowFlags)
    {
        return ImGui::Begin(title, nullptr, static_cast<ImGuiWindowFlags>(windowFlags));
    }

    void GUI::EndWindow()
    {
        ImGui::End();
    }

    bool GUI::BeginTree(const void* pointerID, const TreeFlags treeFlags, const char* text, ...)
    {
        va_list args;
        va_start(args, text);
        auto result = ImGui::TreeNodeExV(pointerID, static_cast<ImGuiTreeNodeFlags>(treeFlags), text, args);
        va_end(args);
        return result;
    }
    
    bool GUI::BeginTree(const char* text, const TreeFlags treeFlags)
    {
        return ImGui::TreeNodeEx(text, static_cast<ImGuiTreeNodeFlags>(treeFlags));
    }

    void GUI::EndTree()
    {
        ImGui::TreePop();
    }
    
    bool GUI::BeginPopup(const char* text)
    {
        return ImGui::BeginPopup(text);
    }
    
    void GUI::EndPopup()
    {
        ImGui::EndPopup();
    }

    void GUI::PushStyleColor(const StyleColor styleColor, const Vector4 &color)
    {
        ImGui::PushStyleColor(static_cast<ImGuiCol>(styleColor), TO_IM_VEC4(color));
    }

    void GUI::PopStyleColor(const uint32 count)
    {
        ImGui::PopStyleColor(count);
    }

    void GUI::PushStyleVariable(const StyleVariable styleVariable, const float value)
    {
        ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleVariable), value);
    }

    void GUI::PushStyleVariable(const StyleVariable styleVariable, const Vector2 &value)
    {
        ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleVariable), TO_IM_VEC2(value));
    }

    void GUI::PopStyleVariable(const uint32 count)
    {
        ImGui::PopStyleVar(count);
    }

    void GUI::PushFont(ImFont *font)
    {
        ImGui::PushFont(font);
    }

    void GUI::PopFont()
    {
        ImGui::PopFont();
    }

    void GUI::PushFontSize(const float size)
    {
        oldFontSize = GetFontSize();
        ImFont* currentFont = ImGui::GetFont();
        currentFont->Scale = size;
        ImGui::PushFont(currentFont);
    }

    void GUI::PopFontSize()
    {
        ImGui::GetFont()->Scale = oldFontSize;
        ImGui::PopFont();
    }

    void GUI::PushDefaultFont()
    {
        PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    }

    void GUI::PushBoldFont()
    {
        PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    }

    void GUI::InsertSeparator()
    {
        ImGui::Separator();
    }

    void GUI::ContinueOnSameLine(float indent)
    {
        ImGui::SameLine(indent);
    }

    void GUI::HorizontalIndent(const float space)
    {
        ImGui::Dummy(ImVec2(space, 0.0f));
    }

    void GUI::HorizontalUnindent(const float space)
    {
        ImGui::Unindent(space);
    }

    void GUI::VerticalIndent(const float space)
    {
        ImGui::Dummy(ImVec2(0.0f, space));
    }

    /* --- SETTER METHODS --- */

    void GUI::SetCurrentPosition(const Vector2 &position)
    {
        ImGui::SetCursorPos(TO_IM_VEC2(position));
    }

    void GUI::SetNextWindowPosition(const Vector2 &position)
    {
        ImGui::SetNextWindowPos(TO_IM_VEC2(position));
    }

    void GUI::SetNextWindowSize(const Vector2 &size)
    {
        ImGui::SetNextWindowSize(TO_IM_VEC2(size));
    }

    void GUI::SetNextWindowSizeConstraints(const Vector2 &minimumSize, const Vector2 &maximumSize)
    {
        ImGui::SetNextWindowSizeConstraints(TO_IM_VEC2(minimumSize), TO_IM_VEC2(maximumSize));
    }

    void GUI::SetNextWindowCollapsed(const bool collapse)
    {
        ImGui::SetNextWindowCollapsed(collapse);
    }

    void GUI::SetNextItemWidth(const float width)
    {
        ImGui::SetNextItemWidth(width);
    }

    void GUI::SetNextItemWidthToFit()
    {
        SetNextItemWidth(GetRemainingHorizontalSpace());
    }

    void GUI::SetNumericInputFormattingSpaceCount(const uint32 spaceCount)
    {
        numericInputFormatting = "." + std::to_string(spaceCount);
    }

    void GUI::ResetNumericInputFormattingSpaceCount()
    {
        numericInputFormatting = ".2";
    }

    void GUI::SetNumericInputStepChange(const float newStepChange)
    {
        numericInputStepChange = newStepChange;
    }

    void GUI::ResetNumericInputStepChange()
    {
        numericInputStepChange = 1.0f;
    }

    void GUI::SetNumericInputDragSpeed(const float newDragSpeed)
    {
        numericInputDragSpeed = newDragSpeed;
    }

    void GUI::ResetNumericInputDragSpeed()
    {
        numericInputDragSpeed = 0.1f;
    }

    void GUI::SetNumericInputLimits(const Optional<float> minimum, const Optional<float> maximum)
    {
        if (minimum.has_value()) numericInputLimitMin = minimum.value();
        if (maximum.has_value()) numericInputLimitMax = maximum.value();
    }

    void GUI::ResetNumericInputLimits()
    {
        numericInputLimitMin = 0.0f;
        numericInputLimitMax = 0.0f;
    }

    void GUI::PushDeactivatedStatus()
    {
        setNextItemDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        GUI::PushStyleVariable(StyleVariable::ALPHA, ImGui::GetStyle().Alpha * 0.5f);
    }

    void GUI::PopDeactivatedStatus()
    {
        setNextItemDisabled = false;
        ImGui::PopItemFlag();
        GUI::PopStyleVariable();
    }

    /* --- GETTER METHODS --- */

    uint32 GUI::GetID(const char* string)
    {
        return ImGui::GetID(string);
    }

    uint32 GUI::HashString(const char* string)
    {
        return ImHashStr(string);
    }

    ImGuiViewport* GUI::GetMainViewport()
    {
        return ImGui::GetMainViewport();
    }

    ImGuiWindow* GUI::GetCurrentWindow()
    {
        return ImGui::GetCurrentWindow();
    }

    ImGuiWindow* GUI::GetHoveredWindow()
    {
        return GImGui->HoveredWindow;
    }

    float GUI::GetFontSize()
    {
        return ImGui::GetFont()->Scale;
    }

    Vector2 GUI::GetRemainingWindowSpace()
    {
        return { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
    }

    float GUI::GetRemainingHorizontalSpace()
    {
        return ImGui::GetContentRegionAvail().x;
    }

    float GUI::GetRemainingVerticalSpace()
    {
        return ImGui::GetContentRegionAvail().y;
    }

    bool GUI::IsItemClicked()
    {
        return ImGui::IsItemClicked(ImGuiMouseButton_Left);
    }

    bool GUI::IsItemDoubleClicked()
    {
        return GUI::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
    }

    bool GUI::IsWindowHovered()
    {
        return ImGui::IsWindowHovered();
    }

    bool GUI::IsItemHovered()
    {
        return ImGui::IsItemHovered();
    }

    ImDrawData* GUI::GetDrawData()
    {
        return ImGui::GetDrawData();
    }

    /* --- EDITOR WIDGETS --- */

    void GUI::BeginProperties()
    {
        // Create table for the property (a cell for the name label and another for displaying its data)
        GenerateID();
        HorizontalIndent(GImGui->Style.IndentSpacing * 0.5f);
        PushStyleVariable(StyleVariable::CELL_PADDING, { GImGui->Style.CellPadding.x, GImGui->Style.CellPadding.y / 2.0f });
        ImGui::BeginTable(IDBuffer, 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV);
        ImGui::TableSetupColumn("PropertyName");
        ImGui::TableSetupColumn("PropertyData", ImGuiTableColumnFlags_WidthStretch);
    }

    void GUI::EndProperties()
    {
        ImGui::EndTable();
        ImGui::PopStyleVar();
        GUI::HorizontalIndent(GImGui->Style.IndentSpacing * 0.5f);
    }

    void GUI::BeginProperty(const char* label, const char* tooltip)
    {
        PushID();

        // Create new row for the property
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::PushID(label);

        // Custom handling of disabled properties, as we do not want the label to be disabled too
        bool makePropertyDisabled = false;
        if (setNextItemDisabled)
        {
            PopDeactivatedStatus();
            makePropertyDisabled = true;
        }

        // Draw label
        GUI::Text("%s", label);

        // Show tooltip if hovered
        if (tooltip != nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            GUI::PushStyleVariable(StyleVariable::WINDOW_ROUNDING, GImGui->Style.FrameRounding * 1.5f);

            ImGui::BeginTooltip();
            ImGui::TextUnformatted(tooltip);
            ImGui::EndTooltip();

            GUI::PopStyleVariable();
        }

        // Go to next column (where the actual property will be drawn) and pre-define its size
        ImGui::TableNextColumn();
        GUI::SetNextItemWidthToFit();

        // Revert changes
        if (makePropertyDisabled) PushDeactivatedStatus();

        GenerateID();
    }

    void GUI::EndProperty()
    {
        ImGui::PopID();
        PopID();
    }

    bool GUI::BeginTreeProperty(const char* label)
    {
        GUI::EndProperties();

        GUI::HorizontalUnindent(GImGui->Style.IndentSpacing * 0.5f);
        bool opened = ImGui::TreeNodeEx(label);
        GUI::HorizontalIndent(GImGui->Style.IndentSpacing * 0.5f);

        GenerateID();
        BeginProperties();

        return opened;
    }

    void GUI::EndTreeProperty()
    {
        EndProperties();

        GUI::EndTree();
        GUI::VerticalIndent(GImGui->Style.ItemSpacing.y);

        GenerateID();
        BeginProperties();
    }

    /* --- EDITOR PROPERTY WIDGETS --- */

    bool GUI::StringProperty(const char* label, String &value, const char* tooltip, const InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = StringInput(IDBuffer, value, inputFlags);
        EndProperty();

        return modified;
    }

    bool GUI::Int32Property(const char* label, int32 &value, const char* tooltip, InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = NumericInput<int32>(IDBuffer, value, true, false, inputFlags, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding);
        EndProperty();

        return modified;
    }

    bool GUI::Int64Property(const char* label, int64 &value, const char* tooltip, const InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = NumericInput<int64>(IDBuffer, value, true, false, inputFlags);
        EndProperty();

        return modified;
    }

    bool GUI::UInt32Property(const char* label, uint32 &value, const char* tooltip, InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = NumericInput<uint32>(IDBuffer, value, true, false, inputFlags);
        EndProperty();

        return modified;
    }

    bool GUI::UInt64Property(const char* label, uint64 &value, const char* tooltip, const InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = NumericInput<uint64>(IDBuffer, value, true, false, inputFlags);
        EndProperty();

        return modified;
    }

    bool GUI::FloatProperty(const char* label, float &value, const char* tooltip, const InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = NumericInput<float>(IDBuffer, value, true, false, inputFlags);
        EndProperty();

        return modified;
    }

    bool
    GUI::DoubleProperty(const char* label, double &value, const char* tooltip, const InputFlags inputFlags)
    {
        BeginProperty(label, tooltip);
        bool modified = NumericInput<double>(IDBuffer, value, true, false, inputFlags);
        EndProperty();

        return modified;
    }

    bool GUI::CheckboxProperty(const char* label, bool &value, const char* tooltip)
    {
        BeginProperty(label, tooltip);
        bool modified = Checkbox(IDBuffer, value);
        EndProperty();

        return modified;
    }

    bool GUI::CheckboxProperty(const char* label, uint32 &value, const char* tooltip)
    {
        BeginProperty(label, tooltip);
        bool modified = Checkbox(IDBuffer, value);
        EndProperty();

        return modified;
    }

    bool GUI::DropdownProperty(const char* label, uint32 &value, const char* *options, const uint32 optionCount, const bool* deactivatedFlags, const char* tooltip)
    {
        BeginProperty(label, tooltip);
        bool modified = Dropdown(IDBuffer, value, options, optionCount, deactivatedFlags);
        EndProperty();

        return modified;
    }

    bool GUI::Vector3Property(const char* label, Vector3 &value, const char* tooltip, const float* resetValues)
    {
        BeginProperty(label, tooltip);
        bool modified = Vector3Input(value, resetValues);
        EndProperty();

        return modified;
    }

    bool GUI::TextureProperty(const char* label, SharedPtr<Rendering::Texture> &texture, const Engine::TextureType textureType, const char* tooltip)
    {
        BeginProperty(label, tooltip);
        ImGui::GetCurrentTable()->Flags = ImGuiTableFlags_NoBordersInBody;

        bool changed = false;

        float buttonSize = ImGui::GetFrameHeight() * 3.0f;
        Vector2 xButtonSize = { buttonSize / 4.0f, buttonSize };

        GUI::SetCurrentPosition({ ImGui::GetContentRegionMax().x - buttonSize - xButtonSize.x, ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y });
        GUI::PushStyleVariable(StyleVariable::ITEM_SPACING,    { 0.00f, 0.00f });
        GUI::PushStyleColor(StyleColor::BUTTON,                { 0.25f, 0.25f, 0.25f, 1.00f });
        GUI::PushStyleColor(StyleColor::HOVERED_BUTTON,        { 0.35f, 0.35f, 0.35f, 1.00f });
        GUI::PushStyleColor(StyleColor::ACTIVE_BUTTON,         { 0.25f, 0.25f, 0.25f, 1.00f });

        ImGui::ImageButton(texture->GetImGuiTextureID(), { buttonSize, buttonSize }, { 1, 1 }, { 0, 0 }, 0);

        GUI::PopStyleColor(3);
        GUI::ContinueOnSameLine(0);

        GUI::PushStyleColor(StyleColor::BUTTON,            { 0.20f, 0.20f, 0.20f, 1.00f });
        GUI::PushStyleColor(StyleColor::HOVERED_BUTTON,    { 0.30f, 0.30f, 0.30f, 1.00f });
        GUI::PushStyleColor(StyleColor::ACTIVE_BUTTON,     { 0.20f, 0.20f, 0.20f, 1.00f });

        if (GUI::RoundedButton("x", xButtonSize, CornerRoundingFlags::RIGHT))
        {
            changed = true;
        }

        GUI::PopStyleColor(3);
        GUI::PopStyleVariable();

        EndProperty();

        return changed;
    }
}