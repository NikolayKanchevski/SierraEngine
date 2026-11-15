//
// Created by Nikolay Kanchevski on 14.11.26.
//

#include "TextDialog.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextDialog::TextDialog(const TextDialogCreateInfo& createInfo)
        : title(createInfo.title), text(createInfo.text), confirmationText(createInfo.confirmationText), ValidationCallback(createInfo.ValidationCallback), ActionCallback(createInfo.ActionCallback)
    {
        APP_THROW_IF(createInfo.inputBufferSize == 0, Sierra::InvalidValueError("Cannot create Text Dialog, as inputBufferSize must be greater than [0]"));
        APP_THROW_IF(createInfo.ActionCallback == nullptr, Sierra::InvalidValueError("Cannot create Text Dialog, as ActionCallback must not be a nullptr"));

        inputBuffer.resize(createInfo.inputBufferSize);
        std::ranges::copy(createInfo.placeholder.substr(0, inputBuffer.size()), inputBuffer.begin());
    }

    bool TextDialog::Draw(Sierra::CommandBuffer& commandBuffer)
    {
        bool open = BeginWizard(title);
        if (open)
        {
            ImGui::TextUnformatted(text.data(), text.data() + text.size());

            if (focusInput)
            {
                ImGui::SetKeyboardFocusHere();
                focusInput = false;
            }

            const float32 availableWidth = ImGui::GetContentRegionAvail().x;

            ImGui::SetNextItemWidth(availableWidth);
            const bool submitted = ImGui::InputText("##TextDialogInput", inputBuffer.data(), inputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue);

            constexpr ImColor ERROR_COLOR   = { 0.95f, 0.35f, 0.35f, 1.0f }; // TODO:
            if (!error.empty()) ImGui::TextColored(ERROR_COLOR, "%s!", error.c_str());

            const ImGuiStyle& style = ImGui::GetStyle();
            const float32 buttonWidth = availableWidth / 4 - (2 * style.FramePadding.x + style.ItemSpacing.x);

            const bool confirmed = ImGui::Button(confirmationText.data(), { buttonWidth, 0 }) || submitted;
            ImGui::SameLine();
            const bool cancelled = ImGui::Button("Cancel", { buttonWidth, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_Escape, false);

            const std::string_view input = std::string_view(inputBuffer.data());
            if (confirmed)
            {
                if (ValidationCallback != nullptr)
                {
                    error = ValidationCallback(input);
                }

                if (error.empty())
                {
                    ActionCallback(input);
                    open = false;
                }
            }
            else if (cancelled)
            {
                open = false;
            }

            EndWizard();
        }

        return open;
    }

}
