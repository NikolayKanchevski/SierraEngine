//
// Created by Nikolay Kanchevski on 7.11.24.
//

#pragma once

#include "../Core/ApplicationFrame.h"

namespace SierraEngine
{

    class Modal
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual bool Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) = 0;

        /* --- COPY SEMANTICS --- */
        Modal(const Modal&) = delete;
        Modal& operator=(const Modal&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Modal() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        Modal() noexcept = default;

        /* --- POLLING METHODS --- */
        bool BeginWizard(std::string_view title) const noexcept;
        void EndWizard() const noexcept;

        /* --- MOVE SEMANTICS --- */
        Modal(Modal&&) noexcept = default;
        Modal& operator=(Modal&&) noexcept = default;

    };

}