//
// Created by Nikolay Kanchevski on 28.10.24.
//

#include "ImGuiWidgets.h"

namespace SierraEngine
{

    namespace
    {
        constexpr float32 DRAG_MOUSE_THRESHOLD_FACTOR = 0.50f;

        bool FilePathInput(const std::string_view label, const std::filesystem::path& value, const float32 width)
        {
            const ImGuiStyle& style = ImGui::GetStyle();

            constexpr std::string_view BROWSE_BUTTON_LABEL = "Browse";
            float32 buttonWidth = ImGui::CalcTextSize(BROWSE_BUTTON_LABEL.begin(), BROWSE_BUTTON_LABEL.end()).x + 2 * style.FramePadding.x;

            const float32 filePathWidth = width - (buttonWidth + style.ItemSpacing.x);

            constexpr float32 MAX_BUTTON_WIDTH_OVER_INPUT_FIELD = 0.70f;
            if (buttonWidth > MAX_BUTTON_WIDTH_OVER_INPUT_FIELD * filePathWidth)
            {
                buttonWidth += (filePathWidth + style.ItemSpacing.x);
            }
            else
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                std::string filePathString = value.string();
                ImGuiWidgets::TextInput(label, filePathString, { .width = filePathWidth });
                ImGui::PopItemFlag();
                ImGui::SetNextItemWidth(filePathWidth);
                ImGui::SameLine();
            }

            if (ImGuiWidgets::Button("Browse", { .width = buttonWidth }))
            {
                return true;
            }

            return false;
        }
    }

    void ImGuiWidgets::HorizontalIndent(const float32 indent)
    {
        ImGui::Dummy({ indent, 0.0f });
    }

    void ImGuiWidgets::VerticalIndent(const float32 indent)
    {
        ImGui::Dummy({ 0.0f, indent });
    }

    void ImGuiWidgets::Indent(const Vector2 indent)
    {
        HorizontalIndent(indent.x);
        VerticalIndent(indent.y);
    }

    void ImGuiWidgets::BeginHorizontalPadding(const float32 padding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { padding, 0 });
        ImGui::BeginChild("##HorizontalPadding", { }, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);
        ImGui::PopStyleVar();
    }

    void ImGuiWidgets::BeginVerticalPadding(const float32 padding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, padding });
        ImGui::BeginChild("##VerticalPadding", { }, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);
        ImGui::PopStyleVar();
    }

    void ImGuiWidgets::BeginPadding(const Vector2 padding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
        ImGui::BeginChild("##Padding", { }, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);
        ImGui::PopStyleVar();
    }

    void ImGuiWidgets::EndPadding()
    {
        ImGui::EndChild();
    }

    bool ImGuiWidgets::BeginPropertyTable(const ImGuiPropertyTableInfo& tableInfo)
    {
        if (ImGui::BeginTable("PropertyTable", 2, tableInfo.tableFlags))
        {
            ImGui::TableSetupColumn("PropertyTableLabelColumn", tableInfo.labelColumnFlags);
            ImGui::TableSetupColumn("PropertyTableValueColumn", tableInfo.valueColumnFlags);

            return true;
        }

        return false;
    }

    void ImGuiWidgets::BeginProperty(const std::string_view label, const std::string_view tooltip)
    {
        ImGui::PushID(label.begin(), label.end());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::AlignTextToFramePadding();
        ImGui::TextEx(label.begin(), label.end());

        if (!tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, ImGui::GetStyle().FrameRounding * 1.5f);
            {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(tooltip.data());
                ImGui::EndTooltip();
            }
            ImGui::PopStyleVar();
        }

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    }

    void ImGuiWidgets::EndProperty()
    {
        ImGui::PopID();
    }

    void ImGuiWidgets::EndPropertyTable()
    {
        ImGui::EndTable();
    }

    bool ImGuiWidgets::TextInput(const std::string_view label, std::string& value, const ImGuiTextInputInfo& inputInfo)
    {
        ImGui::SetNextItemWidth(inputInfo.width);
        return ImGui::InputText(label.data(), &value, inputInfo.flags, inputInfo.CharacterCallback, inputInfo.userData);
    }

    bool ImGuiWidgets::FileSelectInput(const std::string_view label, std::filesystem::path& value, const Sierra::PlatformContext& platformContext, const ImGuiFileSelectInputInfo& inputInfo)
    {
        if (!FilePathInput(label, value, inputInfo.width))
        {
            return false;
        }

        const std::optional<std::filesystem::path> filePath = platformContext.OpenSingleFileSelectDialog({
            .message = inputInfo.message,
            .buttonText = inputInfo.buttonText,
            .directoryPath = inputInfo.directoryPath,
            .allowFiles = inputInfo.allowFiles,
            .allowDirectories = inputInfo.allowDirectories,
            .allowedFileExtensions = inputInfo.allowedFileExtensions
        });

        if (!filePath.has_value())
        {
            return false;
        }

        value = std::move(filePath.value());
        return false;
    }

    bool ImGuiWidgets::FileSaveInput(const std::string_view label, std::filesystem::path& value, const Sierra::PlatformContext& platformContext, const ImGuiFileSaveInputInfo& inputInfo)
    {
        if (!FilePathInput(label, value, inputInfo.width))
        {
            return false;
        }

        const std::optional<std::filesystem::path> filePath = platformContext.OpenFileSaveDialog({
            .message = inputInfo.message,
            .buttonText = inputInfo.buttonText,
            .directoryPath = inputInfo.directoryPath,
            .fileName = inputInfo.fileName,
            .allowedFileExtensions = inputInfo.allowedFileExtensions
        });

        if (!filePath.has_value())
        {
            return false;
        }

        value = std::move(filePath.value());
        return false;
    }

    bool ImGuiWidgets::NumericInput(const std::string_view label, void* value, const ImGuiDataType dataType, const ImGuiNumericInputInfo& inputInfo)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label.data());

        const ImVec2 label_size = ImGui::CalcTextSize(label.data(), nullptr, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(inputInfo.width, label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        const bool temp_input_allowed = (inputInfo.flags & ImGuiSliderFlags_NoInput) == 0;
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
            return false;

        // Default format string when passing nullptr
        const char* format = !inputInfo.format.empty() ? inputInfo.format.data() : nullptr;
        if (format == nullptr) format = ImGui::DataTypeGetInfo(dataType)->PrintFmt;

        const bool hovered = ImGui::ItemHoverable(frame_bb, id, g.LastItemData.ItemFlags);
        bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
        if (!temp_input_is_active)
        {
            // Tabbing or CTRL-clicking on Drag turns it into an InputText
            const bool clicked = hovered && ImGui::IsMouseClicked(0, 0, id);
            const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && ImGui::TestKeyOwner(ImGuiKey_MouseLeft, id));
            const bool make_active = (clicked || double_clicked || g.NavActivateId == id);
            if (make_active && (clicked || double_clicked))
                ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if ((clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                    temp_input_is_active = true;

            // (Optional) simple click (without moving) turns Drag into an InputText
            if (g.IO.ConfigDragClickToInputText && temp_input_allowed && !temp_input_is_active)
                if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * DRAG_MOUSE_THRESHOLD_FACTOR))
                {
                    g.NavActivateId = id;
                    g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
                    temp_input_is_active = true;
                }

            if (make_active && !temp_input_is_active)
            {
                ImGui::SetActiveID(id, window);
                ImGui::SetFocusID(id, window);
                ImGui::FocusWindow(window);
                g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        if (temp_input_is_active)
        {
            // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
            const bool is_clamp_input = (inputInfo.flags & ImGuiSliderFlags_AlwaysClamp) != 0 && (inputInfo.min == nullptr || inputInfo.max == nullptr || ImGui::DataTypeCompare(dataType, inputInfo.min, inputInfo.max) < 0);
            return ImGui::TempInputScalar(frame_bb, id, label.data(), dataType, value, format, is_clamp_input ? inputInfo.min : nullptr, is_clamp_input ? inputInfo.max : nullptr);
        }

        // Draw frame
        const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        ImGui::RenderNavHighlight(frame_bb, id);
        ImGui::GetWindowDrawList()->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, inputInfo.rounding, inputInfo.drawFlags);

        float32 speed = 1.0f;
        if (inputInfo.step != nullptr)
        {
            switch (dataType)
            {
                case ImGuiDataType_S8:      { speed = static_cast<float32>(*reinterpret_cast<const uint8*>(inputInfo.step));   break; }
                case ImGuiDataType_U8:      { speed = static_cast<float32>(*reinterpret_cast<const int8*>(inputInfo.step));    break; }
                case ImGuiDataType_S16:     { speed = static_cast<float32>(*reinterpret_cast<const uint16*>(inputInfo.step));  break; }
                case ImGuiDataType_U16:     { speed = static_cast<float32>(*reinterpret_cast<const int16*>(inputInfo.step));   break; }
                case ImGuiDataType_S32:     { speed = static_cast<float32>(*reinterpret_cast<const uint32*>(inputInfo.step));  break; }
                case ImGuiDataType_U32:     { speed = static_cast<float32>(*reinterpret_cast<const int32*>(inputInfo.step));   break; }
                case ImGuiDataType_S64:     { speed = static_cast<float32>(*reinterpret_cast<const uint64*>(inputInfo.step));  break; }
                case ImGuiDataType_U64:     { speed = static_cast<float32>(*reinterpret_cast<const int64*>(inputInfo.step));   break; }
                case ImGuiDataType_Double:  { speed = static_cast<float32>(*reinterpret_cast<const float64*>(inputInfo.step)); break; }
                default:                    { break; }
            }
        }

        // Drag behavior
        const bool value_changed = ImGui::DragBehavior(id, dataType, value, speed, inputInfo.min, inputInfo.max, format, inputInfo.flags);
        if (value_changed) ImGui::MarkItemEdited(id);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), dataType, value, format);
        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("{", "}");
        ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, nullptr, ImVec2(0.5f, 0.5f));

        if (label_size.x > 0.0f)
            ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label.data());

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
        return value_changed;
    }

    bool ImGuiWidgets::NumericEnterInput(const std::string_view label, void* value, const ImGuiDataType dataType, const ImGuiNumericInputInfo& inputInfo)
    {
        const bool result = ImGui::InputScalar(label.data(), dataType, value, inputInfo.step, nullptr, inputInfo.format.data(), inputInfo.flags);
        ImGui::DataTypeClamp(dataType, value, inputInfo.min, inputInfo.max);
        return result;
    }

    bool ImGuiWidgets::VectorInput(const std::string_view label, void* value, const size length, const ImGuiDataType dataType, const ImGuiVectorInputInfo& inputInfo)
    {
        ImGui::PushID(label.begin(), label.end());
        const float32 buttonWidth = ImGui::GetFrameHeight();

        ImDrawFlags inputFieldRounding = ImDrawFlags_RoundCornersRight;
        float32 inputFieldWidth = (inputInfo.width - static_cast<float32>(length - 1) * ImGui::GetStyle().ItemSpacing.x) / static_cast<float32>(length) - buttonWidth;

        bool drawButtons = true;
        bool splitIntoRows = false;

        constexpr float32 MAX_BUTTON_WIDTH_OVER_INPUT_FIELD = 0.7f;
        if (buttonWidth > MAX_BUTTON_WIDTH_OVER_INPUT_FIELD * inputFieldWidth)
        {
            if (length != 2 && length % 2 == 0)
            {
                splitIntoRows = true;
                inputFieldWidth += ImGui::GetStyle().ItemSpacing.x + inputFieldWidth + buttonWidth;

                if (buttonWidth > MAX_BUTTON_WIDTH_OVER_INPUT_FIELD * inputFieldWidth)
                {
                    drawButtons = false;
                }
            }
            else
            {
                drawButtons = false;
            }
        }

        std::string_view format = { };
        if (dataType == ImGuiDataType_Float || dataType == ImGuiDataType_Double)
        {
            const float32 characterWidth = ImGui::CalcTextSize("0").x;
            format = (characterWidth < inputFieldWidth / 7.0f) ? "%.2f" : "%.1f";
        }

        if (!drawButtons)
        {
            inputFieldWidth += buttonWidth;
            inputFieldRounding = ImDrawFlags_RoundCornersAll;
        }

        bool modified = false;
        const size dataTypeSize = ImGui::DataTypeGetInfo(dataType)->Size;
        for (size i = 0; i < length; i++)
        {
            constexpr std::array<char, 4> SYMBOLS = { 'X', 'Y', 'Z', 'W' };
            if (drawButtons)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y / 2.0f });

                constexpr std::array<ImVec4, 4> BUTTON_COLORS
                {
                    ImVec4 { 0.80f, 0.10f, 0.15f, 1.00f },
                    ImVec4 { 0.20f, 0.70f, 0.20f, 1.00f },
                    ImVec4 { 0.10f, 0.25f, 0.80f, 1.00f },
                    ImVec4 { 0.60f, 0.07f, 0.60f, 1.00f }
                };

                ImGui::PushStyleColor(ImGuiCol_Button, BUTTON_COLORS[i]);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BUTTON_COLORS[i] + ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, BUTTON_COLORS[i]);

                const char symbol[2] = { SYMBOLS[i % SYMBOLS.size()], '\0' };
                if (Button(std::string_view(symbol, 1), { .width = buttonWidth, .drawFlags = ImDrawFlags_RoundCornersLeft }))
                {
                    std::memcpy(reinterpret_cast<uint8*>(value) + (i * dataTypeSize), reinterpret_cast<const uint8*>(inputInfo.resetValues) + (i * dataTypeSize), dataTypeSize);
                    modified = true;
                }

                ImGui::PopStyleColor(3);
                ImGui::SameLine();
            }

            const ImGuiNumericInputInfo numericInputInfo
            {
                .step = inputInfo.step,
                .min = inputInfo.min,
                .max = inputInfo.max,
                .format = format,
                .flags = inputInfo.flags,
                .width = inputFieldWidth,
                .drawFlags = inputFieldRounding,
            };

            ImGui::SetNextItemWidth(inputFieldWidth);
            const char ID[4] = { '#', '#', SYMBOLS[i], '\0' };
            if (NumericInput(ID, reinterpret_cast<uint8*>(value) + (i * dataTypeSize), dataType, numericInputInfo))
            {
                modified = true;
            }

            if (drawButtons)
            {
                ImGui::PopStyleVar();
            }

            if (i % 2 == 0 || !splitIntoRows)
            {
                ImGui::SameLine();
            }
        }

        ImGui::PopID();
        return modified;
    }

    bool ImGuiWidgets::ColorInput(const std::string_view label, ColorRGB& value, const ImGuiColorInputInfo& inputInfo)
    {
        return ImGui::ColorEdit3(label.data(), &value[0], inputInfo.flags);
    }

    bool ImGuiWidgets::ColorInput(const std::string_view label, ColorRGBA& value, const ImGuiColorInputInfo& inputInfo)
    {
        return ImGui::ColorEdit4(label.data(), &value[0], inputInfo.flags);
    }

    bool ImGuiWidgets::ImageInput(const std::string_view label, std::filesystem::path& value, const Sierra::PlatformContext& platformContext, const ImGuiImageInputInfo& inputInfo)
    {
        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (!window || window->SkipItems) return false;

        const float32 size = inputInfo.width;
        const ImRect backgroundRect = ImRect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(size, size));
        ImGui::ItemSize(backgroundRect);

        const ImGuiStyle& style = ImGui::GetStyle();
        window->DrawList->AddRectFilled(backgroundRect.Min, backgroundRect.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
        ImGui::RenderFrameBorder(backgroundRect.Min, backgroundRect.Max, style.FrameRounding);

        if (inputInfo.previewID.has_value())
        {
            const float32 scaling = glm::min(
                size / static_cast<float32>(inputInfo.imageWidth),
                size / static_cast<float32>(inputInfo.imageHeight)
            );

            ImRect imageRect = { };
            constexpr float32 PADDING = 2.0f;
            switch (inputInfo.fitType)
            {
                case ImGuiImageInputFitType::Fit:
                {
                    const float32 width = static_cast<float32>(inputInfo.imageWidth) * scaling;
                    const float32 height = static_cast<float32>(inputInfo.imageHeight) * scaling;

                    const float32 widthPadding = (size - width) / 2.0f + PADDING;
                    const float32 heightPadding = (size - height) / 2.0f + PADDING;
                    imageRect = ImRect(ImVec2(backgroundRect.Min.x + widthPadding, backgroundRect.Min.y + heightPadding), ImVec2(backgroundRect.Max.x - widthPadding, backgroundRect.Max.y - heightPadding));

                    break;
                }
                case ImGuiImageInputFitType::Stretch:
                {
                    ImRect(ImVec2(backgroundRect.Min.x + PADDING, backgroundRect.Min.y + PADDING), ImVec2(backgroundRect.Max.x - PADDING, backgroundRect.Max.y - PADDING));
                    break;
                }
            }

                window->DrawList->AddImage(ImTextureRef(inputInfo.previewID.value()), imageRect.Min, imageRect.Max);
        }

        constexpr std::string_view BROWSE_BUTTON_LABEL = "Browse";
        const float32 browseButtonWidth = ImGui::CalcTextSize(BROWSE_BUTTON_LABEL.begin(), BROWSE_BUTTON_LABEL.end()).x + 2 * style.FramePadding.x;

        ImGuiWidgets::BeginHorizontalPadding((size - browseButtonWidth) / 2.0f);

        const ImGuiFileSelectInputInfo filePathInputInfo =
        {
            .directoryPath = inputInfo.directoryPath,
            .allowedFileExtensions = inputInfo.allowedFileExtensions,
            .width = browseButtonWidth
        };
        const bool result = ImGuiWidgets::FileSelectInput("##ImageInputFileSelectInput", value, platformContext, filePathInputInfo);

        ImGuiWidgets::EndPadding();
        return result;
    }

    bool ImGuiWidgets::Button(const std::string_view label, const ImGuiButtonInfo& buttonInfo)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label.data());
        const ImVec2 label_size = ImGui::CalcTextSize(label.data(), nullptr, true);

        ImVec2 position = window->DC.CursorPos;
        if ((buttonInfo.flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
        {
            position.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
        }
        ImVec2 size = ImGui::CalcItemSize({ buttonInfo.width, ImGui::GetFrameHeight() }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        const ImRect bb(position, position + size);
        ImGui::ItemSize(size, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, buttonInfo.flags);

        // Render
        const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        ImGui::RenderNavHighlight(bb, id);
        ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

        if (g.LogEnabled) ImGui::LogSetNextTextDecoration("[", "]");
        ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, col, buttonInfo.rounding, buttonInfo.drawFlags);
        ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label.data(), nullptr, &label_size, style.ButtonTextAlign, &bb);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
        return pressed;
    }

    bool ImGuiWidgets::Dropdown(const std::string_view label, uint32& value, const ImGuiDropdownInfo& dropdownInfo)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.WindowFlags;
        g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
        if (window->SkipItems)
            return false;

        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label.data());
        IM_ASSERT((dropdownInfo.flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together
        if (dropdownInfo.flags & ImGuiComboFlags_WidthFitPreview)
            IM_ASSERT((dropdownInfo.flags & (ImGuiComboFlags_NoPreview | (ImGuiComboFlags)ImGuiComboFlags_CustomPreview)) == 0);

        const char* preview_value = dropdownInfo.options[value].text.data();
        const float arrow_size = (dropdownInfo.flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
        const ImVec2 label_size = ImGui::CalcTextSize(label.data(), nullptr, true);
        const float preview_width = ((dropdownInfo.flags & ImGuiComboFlags_WidthFitPreview) && (preview_value != nullptr)) ? ImGui::CalcTextSize(preview_value, nullptr, true).x : 0.0f;
        const float w = (dropdownInfo.flags & ImGuiComboFlags_NoPreview) ? arrow_size : ((dropdownInfo.flags & ImGuiComboFlags_WidthFitPreview) ? (arrow_size + preview_width + style.FramePadding.x * 2.0f) : dropdownInfo.width);
        const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(bb.Min, bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id, &bb))
            return false;

        // Open on click
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
        bool popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);
        if (pressed && !popup_open)
        {
            ImGui::OpenPopupEx(popup_id, ImGuiPopupFlags_None);
            popup_open = true;
        }

        // Render shape
        const ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
        ImGui::RenderNavHighlight(bb, id);
        if (!(dropdownInfo.flags & ImGuiComboFlags_NoPreview))
            window->DrawList->AddRectFilled(bb.Min, ImVec2(value_x2, bb.Max.y), frame_col, style.FrameRounding, (dropdownInfo.flags & ImGuiComboFlags_NoArrowButton) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);
        if (!(dropdownInfo.flags & ImGuiComboFlags_NoArrowButton))
        {
            ImU32 bg_col = ImGui::GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
            window->DrawList->AddRectFilled(ImVec2(value_x2, bb.Min.y), bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);
            if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
                ImGui::RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
        }
        ImGui::RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);

        // Custom preview
        if (dropdownInfo.flags & ImGuiComboFlags_CustomPreview)
        {
            g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
            IM_ASSERT(preview_value == nullptr || preview_value[0] == 0);
            preview_value = nullptr;
        }

        // Render preview and label
        if (preview_value != nullptr && !(dropdownInfo.flags & ImGuiComboFlags_NoPreview))
        {
            if (g.LogEnabled)
                ImGui::LogSetNextTextDecoration("{", "}");

            ImGui::RenderTextClipped(bb.Min + style.FramePadding, ImVec2(value_x2, bb.Max.y), preview_value, nullptr, nullptr, { 0.5f + (arrow_size / ImGui::GetContentRegionAvail().x), 0.0f });
        }

        if (label_size.x > 0)
            ImGui::RenderText(ImVec2(bb.Max.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), label.data());

        if (!popup_open)
            return false;

        g.NextWindowData.WindowFlags = backup_next_window_data_flags;

        bool modified = false;
        if (ImGui::BeginComboPopup(popup_id, bb, dropdownInfo.flags))
        {
            for (size i = 0; i < dropdownInfo.options.size(); i++)
            {
                const ImGuiDropdownOption& option = dropdownInfo.options[i];
                bool selected = value == i;

                if (option.disabled)
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                }

                if (ImGui::Selectable(option.text.data(), selected) && !selected)
                {
                    value = i;
                    modified = true;
                }
                if (selected) ImGui::SetItemDefaultFocus();

                if (option.disabled)
                {
                    ImGui::PopItemFlag();
                }
            }

        }
        ImGui::EndCombo();

        return modified;
    }

}