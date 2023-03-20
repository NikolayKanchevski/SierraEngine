//
// Created by Nikolay Kanchevski on 15.01.23.
//

#include "ImGuiUtilities.h"

    using namespace ImGui;

    const uint boldFontIndex = 1;
    static int s_UIContextID = 0;
    static int s_Counter = 0;
    char IDBuffer[16];
    static bool setNextItemDisabled = false;

    static float dataLimitMin = 0.0f;
    static float dataLimitMax = 0.0f;
    static float dataDragSpeed = 0.1f;
    static float step = 1.0f;
    static float stepFast = 10.0f;
    static String format = ".2";

    static bool drawnAnyProperties = false;
    static ImGuiTableFlags currentPropertyTableFlags = 0;

    void GUI::GenerateID()
    {
        IDBuffer[0] = '#';
        IDBuffer[1] = '#';
        memset(IDBuffer + 2, 0, 14);
        ++s_Counter;
        String buffer = FORMAT_STRING("##{0}", s_Counter);
        std::memcpy(&IDBuffer, buffer.data(), 16);
    }

    void GUI::ExternalPushID()
    {
        ++s_UIContextID;
        ImGui::PushID(s_UIContextID);
        s_Counter = 0;
    }

    void GUI::ExternalPopID()
    {
        ImGui::PopID();
        --s_UIContextID;
    }

    void GUI::VerticalIndent(const float height)
    {
        ImGui::Dummy(ImVec2(0.0f, height));
    }

    void GUI::PushDeactivatedStatus()
    {
        setNextItemDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    void GUI::PopDeactivatedStatus()
    {
        setNextItemDisabled = false;
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    void GUI::SetNumericFormattingSpaces(const uint spacesCount)
    {
        format = "." + std::to_string(spacesCount);
    }

    void GUI::ResetNumericFormattingSpaces()
    {
        format = ".2";
    }

    void GUI::SetNumericStepChange(float newStep)
    {
        step = newStep;
    }

    void GUI::ResetNumericStepChange()
    {
        step = 1.0f;
    }

    void GUI::SetInputLimits(Vector2 limits)
    {
        dataLimitMin = limits.x;
        dataLimitMax = limits.y;
    }

    void GUI::ResetInputLimits()
    {
        dataLimitMin = 0.0f;
        dataLimitMax = 0.0f;
    }

    void GUI::CustomLabel(const char* label)
    {
        ImGui::Text("%s", label);
    }

    void GUI::ShowTooltip(const char* tooltip)
    {
        if (tooltip && ImGui::IsItemHovered() && GImGui->HoveredIdTimer > HOVER_TIME_THRESHOLD)
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(tooltip);
            ImGui::EndTooltip();
        }
    }

    bool GUI::BeginWindow(const char* title, bool *open, const ImGuiWindowFlags windowFlags)
    {
        return ImGui::Begin(title, open, windowFlags);
    }

    void GUI::EndWindow()
    {
        ImGui::End();
    }

    void GUI::BoldText(const char* text)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[boldFontIndex]);
        ImGui::Text("%s", text);
        ImGui::PopFont();
    }

    bool GUI::RoundedButton(const char *label, const ImVec2 &givenSize, const ImDrawFlags roundingType, const float rounding, ImGuiButtonFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        ImVec2 pos = window->DC.CursorPos;
        if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
            pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
        ImVec2 size = CalcItemSize(givenSize, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        const ImRect bb(pos, pos + size);
        ItemSize(size, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
            flags |= ImGuiButtonFlags_Repeat;

        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

        // Render
        const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        RenderNavHighlight(bb, id);
        ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, col, rounding, roundingType);

        if (g.LogEnabled)
            LogSetNextTextDecoration("[", "]");

        RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

        return pressed;
    }

    void GUI::BeginProperties(const ImGuiTableFlags tableFlags)
    {
        bool statusChanged = false;
        if (setNextItemDisabled)
        {
            PopDeactivatedStatus();
            statusChanged = true;
        }

        GenerateID();
        ImGui::Unindent(GImGui->Style.IndentSpacing * 0.5);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { GImGui->Style.CellPadding.x, GImGui->Style.CellPadding.y / 2.0f });
        ImGui::BeginTable(IDBuffer, 2, tableFlags | ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("PropertyName");
        ImGui::TableSetupColumn("PropertyData", ImGuiTableColumnFlags_WidthStretch);
        currentPropertyTableFlags = tableFlags;

        if (statusChanged)
        {
            PushDeactivatedStatus();
        }
    }

    void GUI::EndProperties()
    {
        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::Indent(GImGui->Style.IndentSpacing * 0.5f);
        drawnAnyProperties = false;
    }

    void GUI::BeginProperty(const char *label)
    {
        ExternalPushID();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::PushID(label);

        bool statusChanged = false;
        if (setNextItemDisabled)
        {
            PopDeactivatedStatus();
            statusChanged = true;
        }

        ImGui::Text("%s", label);

        if (statusChanged)
        {
            PushDeactivatedStatus();
        }

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        GenerateID();

        drawnAnyProperties = true;
    }

    void GUI::EndProperty()
    {
        ImGui::PopID();
        ExternalPopID();
    }

    bool GUI::BeginTreeProperties(const char* label)
    {
        GUI::EndProperties();

        ImGui::Unindent(GImGui->Style.IndentSpacing * 0.5f);
        bool opened = ImGui::TreeNodeEx(label);
        ImGui::Indent(GImGui->Style.IndentSpacing * 0.5);

        GenerateID();
        BeginProperties();
        drawnAnyProperties = true;

        return opened;
    }

    void GUI::EndTreeProperties()
    {
        EndProperties();

        ImGui::TreePop();
        GUI::VerticalIndent(GImGui->Style.ItemSpacing.y);

        GenerateID();
        BeginProperties(currentPropertyTableFlags);
    }

    void GUI::PropertyTabHeader(const char* label)
    {
        bool hadDrawnAnyProperties = drawnAnyProperties;
        EndProperties();

        if (hadDrawnAnyProperties) GUI::VerticalIndent(GImGui->Style.ItemSpacing.y);
        GUI::BoldText(label);

        GenerateID();
        BeginProperties(currentPropertyTableFlags);
    }

    bool GUI::StringInput(const char* labelID, String &value, const ImGuiInputTextFlags inputFlags)
    {
        return ImGui::InputText(labelID, &value, 0, nullptr);
    }

    template<typename T, ENABLE_IF(std::is_arithmetic_v<T>)>
    static inline ImGuiDataType GetNumericImGuiDataType()
    {
        if (std::is_same<T, int>()) return ImGuiDataType_S32;
        if (std::is_same<T, uint>()) return ImGuiDataType_U32;
        if (std::is_same<T, int64>()) return ImGuiDataType_S64;
        if (std::is_same<T, uint64>()) return ImGuiDataType_U64;
        if (std::is_same<T, float>()) return ImGuiDataType_Float;
        if (std::is_same<T, double>()) return ImGuiDataType_Double;
        return -1;
    }

    template<typename T, ENABLE_IF(std::is_arithmetic_v<T>)>
    static inline String GetSuitableFormat()
    {
        auto dataType = GetNumericImGuiDataType<T>();
        String dataTypeFormatting = String(ImGui::DataTypeGetInfo(dataType)->ScanFmt);

        dataTypeFormatting = dataTypeFormatting.insert(1, format);
        return dataTypeFormatting;
    }

    template<typename T, ENABLE_IF(std::is_arithmetic_v<T>)>
    static inline bool NumericInput(const char* labelID, T &valueReference, bool canDrag, bool centerAlign, ImDrawFlags roundingType = ImDrawFlags_RoundCornersAll, float rounding = GImGui->Style.FrameRounding, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None)
    {
        ImGuiDataType dataType = GetNumericImGuiDataType<T>();
        auto value = &valueReference;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiStyle& style = g.Style;

        std::string formatString = GetSuitableFormat<T>();
        const char* localFormat = formatString.c_str();

        if (!canDrag)
        {
            char buf[64];
            DataTypeFormatString(buf, IM_ARRAYSIZE(buf), dataType, value, localFormat);

            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
                flags |= ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoMarkEdited; // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.
            #pragma clang diagnostic pop

            bool value_changed = false;
            if (step != (T) 0)
            {
                const float button_size = GetFrameHeight();

                BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
                PushID(labelID);
                SetNextItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
                if (InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
                    value_changed = DataTypeApplyFromText(buf, dataType, value, localFormat);
                IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags);

                // Step buttons
                const ImVec2 backup_frame_padding = style.FramePadding;
                style.FramePadding.x = style.FramePadding.y;
                ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;
                if (flags & ImGuiInputTextFlags_ReadOnly)
                    BeginDisabled();
                SameLine(0, style.ItemInnerSpacing.x);
                if (ButtonEx("-", ImVec2(button_size, button_size), button_flags))
                {
                    valueReference -= g.IO.KeyCtrl ? (T) stepFast : (T) step;
                    value_changed = true;
                }
                SameLine(0, style.ItemInnerSpacing.x);
                if (ButtonEx("+", ImVec2(button_size, button_size), button_flags))
                {
                    valueReference += g.IO.KeyCtrl ? (T) stepFast : (T) step;
                    value_changed = true;
                }
                if (flags & ImGuiInputTextFlags_ReadOnly)
                    EndDisabled();

                const char* label_end = FindRenderedTextEnd(labelID);
                if (labelID != label_end)
                {
                    SameLine(0, style.ItemInnerSpacing.x);
                    TextEx(labelID, label_end);
                }
                style.FramePadding = backup_frame_padding;

                PopID();
                EndGroup();
            }
            else
            {
                if (InputText(labelID, buf, IM_ARRAYSIZE(buf), flags))
                    value_changed = DataTypeApplyFromText(buf, dataType, value, localFormat);
            }
            if (value_changed)
                MarkItemEdited(g.LastItemData.ID);

            return value_changed;
        }
        else
        {
            const ImGuiID id = window->GetID(labelID);
            const float w = CalcItemWidth();

            const ImVec2 label_size = CalcTextSize(labelID, NULL, true);
            const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
            const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

            const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
            ItemSize(total_bb, style.FramePadding.y);
            if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
                return false;

            const bool hovered = ItemHoverable(frame_bb, id);
            bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
            if (!temp_input_is_active)
            {
                // Tabbing or CTRL-clicking on Drag turns it into an InputText
                const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
                const bool clicked = (hovered && g.IO.MouseClicked[0]);
                const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2);
                const bool make_active = (input_requested_by_tabbing || clicked || double_clicked || g.NavActivateId == id);
                if (make_active && temp_input_allowed)
                    if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                        temp_input_is_active = true;

                // (Optional) simple click (without moving) turns Drag into an InputText
                if (g.IO.ConfigDragClickToInputText && temp_input_allowed && !temp_input_is_active)
                    if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * 0.50f)) // DRAG_MOUSE_THRESHOLD_FACTOR
                    {
                        g.NavActivateId = id;
                        g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
                        temp_input_is_active = true;
                    }

                if (make_active && !temp_input_is_active)
                {
                    SetActiveID(id, window);
                    SetFocusID(id, window);
                    FocusWindow(window);
                    g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
                }
            }

            if (temp_input_is_active)
            {
                // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
                const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0 && DataTypeCompare(dataType, &dataLimitMin, &dataLimitMax) < 0;
                return TempInputScalar(frame_bb, id, labelID, dataType, value, localFormat, is_clamp_input ? &dataLimitMin : NULL, is_clamp_input ? &dataLimitMax : NULL);
            }

            // Draw frame
            const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
            RenderNavHighlight(frame_bb, id);
            ImGui::GetWindowDrawList()->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, rounding, roundingType);

            // Drag behavior
            const bool value_changed = DragBehavior(id, dataType, value, dataDragSpeed, &dataLimitMin, &dataLimitMax, localFormat, flags);
            if (value_changed)
                MarkItemEdited(id);

            // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
            char value_buf[64];
            const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), dataType, value, localFormat);
            if (g.LogEnabled)
                LogSetNextTextDecoration("{", "}");

            float xAlign = centerAlign ? 0.5f : g.Style.ItemInnerSpacing.x / ImGui::GetItemRectSize().x;
            float yAlign = centerAlign ? 0.5f : 0.0f;
            RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(xAlign, yAlign));

            if (label_size.x > 0.0f)
                RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), labelID);

            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
            return value_changed;
        }

    }

    bool GUI::IntInput(const char* labelID, int &value, bool canDrag, const ImGuiInputTextFlags inputFlags)
    {
        return NumericInput<int>(labelID, value, canDrag, false, inputFlags);
    }

    bool GUI::Int64Input(const char* labelID, int64 &value, bool canDrag, const ImGuiInputTextFlags inputFlags)
    {
        return NumericInput<int64>(labelID, value, canDrag, false, inputFlags);
    }

    bool GUI::UIntInput(const char* labelID, uint &value, bool canDrag, const ImGuiInputTextFlags inputFlags)
    {
        return NumericInput<uint>(labelID, value, canDrag, false, inputFlags);
    }

    bool GUI::UInt64Input(const char* labelID, uint64 &value, bool canDrag, const ImGuiInputTextFlags inputFlags)
    {
        return NumericInput<uint64>(labelID, value, canDrag, false, inputFlags);
    }

    bool GUI::FloatInput(const char* labelID, float &value, bool canDrag, const ImGuiInputTextFlags inputFlags)
    {
        return NumericInput<float>(labelID, value, canDrag, false, inputFlags);
    }

    bool GUI::DoubleInput(const char* labelID, double &value, bool canDrag, const ImGuiInputTextFlags inputFlags)
    {
        return NumericInput<double>(labelID, value, canDrag, false, inputFlags);
    }

    bool GUI::Checkbox(const char* labelID, bool &value)
    {
        return ImGui::Checkbox(labelID, &value);
    }

    bool GUI::Vector3Input(Vector3 &value, const float *resetValues, const char** tooltips)
    {
        auto boldFont = ImGui::GetIO().Fonts->Fonts[boldFontIndex];

        float frameHeight = ImGui::GetFrameHeight();
        ImVec2 buttonSize = { frameHeight, frameHeight };

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
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
                ImGui::PushFont(boldFont);

                if (GUI::RoundedButton("X", buttonSize, ImDrawFlags_RoundCornersLeft))
                {
                    value.x = resetValues[0];
                    modified = true;
                }

                ImGui::PopFont();
                ImGui::PopStyleColor(4);

                ImGui::SameLine();
            }

            ImGui::SetNextItemWidth(inputFieldWidth);
            if (NumericInput<float>("##X", value.x, true, true, inputFieldRounding))
            {
                modified = true;
            }

            if (tooltips) ShowTooltip(tooltips[0]);

            if (drawButtons) ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        // Y
        {
            if (drawButtons)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
                ImGui::PushFont(boldFont);

                if (GUI::RoundedButton("Y", buttonSize, ImDrawFlags_RoundCornersLeft))
                {
                    value.y = resetValues[1];
                    modified = true;
                }

                ImGui::PopFont();
                ImGui::PopStyleColor(4);

                ImGui::SameLine();
            }

            ImGui::SetNextItemWidth(inputFieldWidth);
            if (NumericInput<float>("##Y", value.y, true, true, inputFieldRounding))
            {
                modified = true;
            }

            if (tooltips) ShowTooltip(tooltips[1]);

            if (drawButtons) ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        // Z
        {
            if (drawButtons)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
                ImGui::PushFont(boldFont);

                if (GUI::RoundedButton("Z", buttonSize, ImDrawFlags_RoundCornersLeft))
                {
                    value.z = resetValues[2];
                    modified = true;
                }

                ImGui::PopFont();
                ImGui::PopStyleColor(4);

                ImGui::SameLine();
            }

            ImGui::SetNextItemWidth(inputFieldWidth);
            if (NumericInput<float>("##Z", value.z, true, true, inputFieldRounding))
            {
                modified = true;
            }

            if (tooltips) ShowTooltip(tooltips[2]);

            if (drawButtons) ImGui::PopStyleVar();
        }

        return modified;
    }

    bool GUI::Dropdown(const char* labelID, uint &value, const char** options, uint optionsCount, const bool* deactivatedFlags)
    {
        bool modified = false;
        const char* currentOption = options[value];

        if (ImGui::BeginCombo(labelID, currentOption))
        {
            for (uint i = 0; i < optionsCount; i++)
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

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }

                if (deactivated)
                {
                    PopDeactivatedStatus();
                }
            }

            ImGui::EndCombo();
        }

        return modified;
    }


    bool GUI::StringProperty(const char* label, String &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = StringInput(IDBuffer, value, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::IntProperty(const char* label, int &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = NumericInput<int>(IDBuffer, value, true, false, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::Int64Property(const char* label, int64 &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = NumericInput<int64>(IDBuffer, value, true, false, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::UIntProperty(const char* label, uint &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = NumericInput<uint>(IDBuffer, value, true, false, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::UInt64Property(const char* label, uint64 &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = NumericInput<uint64>(IDBuffer, value, true, false, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::FloatProperty(const char* label, float &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = NumericInput<float>(IDBuffer, value, true, false, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::DoubleProperty(const char* label, double &value, const char* tooltip, const ImGuiInputTextFlags inputFlags)
    {
        BeginProperty(label);
        bool modified = NumericInput<double>(IDBuffer, value, true, false, ImDrawFlags_RoundCornersAll, GImGui->Style.FrameRounding, inputFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::CheckboxProperty(const char* label, bool &value, const char* tooltip)
    {
        BeginProperty(label);
        bool modified = Checkbox(IDBuffer, value);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::DropdownProperty(const char* label, uint &value, const char** options, const uint optionsCount,  const bool* deactivatedFlags, const char* tooltip)
    {
        BeginProperty(label);
        bool modified = Dropdown(IDBuffer, value, options, optionsCount, deactivatedFlags);
        ShowTooltip(tooltip);
        EndProperty();

        return modified;
    }

    bool GUI::PropertyVector3(const char* label, Vector3 &value, const float *resetValues, const char** tooltips)
    {
        BeginProperty(label);
        bool modified = Vector3Input(value, resetValues, tooltips);
        EndProperty();

        return modified;
    }

    bool GUI::TextureProperty(const char* label, SharedPtr<Texture> &texture, const char* tooltip)
    {
        BeginProperty(label);

        bool changed = false;

        float buttonSize = ImGui::GetFrameHeight() * 3.0f;
        ImVec2 xButtonSize = { buttonSize / 4.0f, buttonSize };

        ImGui::SetCursorPos({ ImGui::GetContentRegionMax().x - buttonSize - xButtonSize.x, ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

        ImGui::ImageButton(texture->GetImGuiTextureID(), { buttonSize, buttonSize }, { 1, 1 }, { 0, 0 }, 0);
        ShowTooltip(tooltip);

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

        if (GUI::RoundedButton("x", xButtonSize, ImDrawFlags_RoundCornersRight))
        {
            texture = Texture::GetDefaultTexture(texture->GetTextureType());
            changed = true;
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        EndProperty();

        return changed;
    }

