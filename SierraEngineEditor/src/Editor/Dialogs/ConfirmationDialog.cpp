//
// Created by Nikolay Kanchevski on 15.11.26.
//

#include "ConfirmationDialog.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    ConfirmationDialog::ConfirmationDialog(const ConfirmationDialogCreateInfo& createInfo)
        : title(createInfo.title), text(createInfo.text), confirmationText(createInfo.confirmationText), destructive(createInfo.destructive), ActionCallback(createInfo.ActionCallback)
    {
        APP_THROW_IF(createInfo.ActionCallback == nullptr, Sierra::InvalidValueError("Cannot create Confirmation Dialog, as ActionCallback must not be a nullptr"));
    }

    /* --- POLLING METHODS --- */

    bool ConfirmationDialog::Draw(const ApplicationFrame& applicationFrame, const DrawFrame&)
    {
        constexpr ImVec4 DESTRUCTIVE_BUTTON_COLOR         = { 0.72f, 0.22f, 0.22f, 1.0f };
        constexpr ImVec4 DESTRUCTIVE_BUTTON_HOVERED_COLOR = { 0.85f, 0.28f, 0.28f, 1.0f };

        bool open = BeginWizard(title);
        if (open)
        {
            ImGui::TextUnformatted(text.data(), text.data() + text.size());
            ImGui::Spacing();

            const float32 availableWidth = ImGui::GetContentRegionAvail().x;
            const ImGuiStyle& style = ImGui::GetStyle();
            const float32 buttonWidth = availableWidth / 4 - (2 * style.FramePadding.x + style.ItemSpacing.x);

            if (destructive)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, DESTRUCTIVE_BUTTON_COLOR);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, DESTRUCTIVE_BUTTON_HOVERED_COLOR);
            }

            // Enter is intentionally not bound to confirm, so a stray keypress cannot trigger the action
            const bool confirmed = ImGui::Button(confirmationText.c_str(), { buttonWidth, 0.0f }) || (applicationFrame.inputManager != nullptr && applicationFrame.inputManager->IsKeyPressed(Sierra::Key::Enter));

            if (destructive) ImGui::PopStyleColor(2);

            ImGui::SameLine();
            const bool cancelled = ImGui::Button("Cancel", { buttonWidth, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_Escape, false);

            if (confirmed)
            {
                ActionCallback();
                open = false;
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
