//
// Created by Nikolay Kanchevski on 15.11.26.
//

#pragma once

#include "../Modal.h"

namespace SierraEngine
{

    using ConfirmationDialogActionCallback = std::function<void()>;

    struct ConfirmationDialogCreateInfo
    {
        std::string_view title = "Confirmation";
        std::string_view text = "Are you sure?";

        std::string_view confirmationText = "Confirm";
        bool destructive = true;

        ConfirmationDialogActionCallback ActionCallback = nullptr;
    };

    class ConfirmationDialog final : public Modal
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ConfirmationDialog(const ConfirmationDialogCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) override;

        /* --- COPY SEMANTICS --- */
        ConfirmationDialog(const ConfirmationDialog&) = delete;
        ConfirmationDialog& operator=(const ConfirmationDialog&) = delete;

        /* --- MOVE SEMANTICS --- */
        ConfirmationDialog(ConfirmationDialog&&) noexcept = default;
        ConfirmationDialog& operator=(ConfirmationDialog&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ConfirmationDialog() noexcept override = default;

    private:
        std::string title = { };
        std::string text = { };
        std::string confirmationText = { };
        bool destructive = false;

        ConfirmationDialogActionCallback ActionCallback = { };

    };

}
