//
// Created by Nikolay Kanchevski on 14.11.26.
//

#pragma once

#include "../EditorModal.h"

namespace SierraEngine
{

    using TextDialogValidationCallback = std::function<std::string(std::string_view input)>;
    using TextDialogActionCallback = std::function<void(std::string_view input)>;

    struct TextDialogCreateInfo
    {
        std::string_view title = "Text Dialog";
        std::string_view text = "Enter text:";

        std::string_view placeholder = { };
        std::string_view confirmationText = "Confirm";

        const TextDialogValidationCallback& ValidationCallback = nullptr;
        const TextDialogActionCallback& ActionCallback;

        size inputBufferSize = 256;
    };

    class TextDialog final : public EditorModal
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextDialog(const TextDialogCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool Draw(Sierra::CommandBuffer& commandBuffer) override;

        /* --- COPY SEMANTICS --- */
        TextDialog(const TextDialog&) = delete;
        TextDialog& operator=(const TextDialog&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextDialog(TextDialog&&) noexcept = default;
        TextDialog& operator=(TextDialog&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~TextDialog() noexcept override = default;

    private:
        std::string title = { };
        std::string text = { };
        std::string confirmationText = { };

        bool focusInput = true;
        std::vector<char> inputBuffer = { };

        std::string error = { };
        TextDialogValidationCallback ValidationCallback = { };
        TextDialogActionCallback ActionCallback = { };

    };

}
