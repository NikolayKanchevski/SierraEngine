//
// Created by Nikolay Kanchevski on 17.11.26.
//

#pragma once
#include <queue>

#include "Modal.h"

namespace SierraEngine
{

    class ModalManager final
    {
    public:
        /* --- CONSTRUCTORS --- */
        ModalManager() noexcept = default;

        /* --- POLLING METHODS --- */
        Modal& PushModal(std::unique_ptr<Modal>&& modal);
        void Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsEmpty() const noexcept { return queue.empty(); }

        /* --- COPY SEMANTICS --- */
        ModalManager(const ModalManager&) = delete;
        ModalManager& operator=(const ModalManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        ModalManager(ModalManager&&) noexcept = default;
        ModalManager& operator=(ModalManager&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ModalManager() noexcept = default;

    private:
        std::queue<std::unique_ptr<Modal>> queue = { };

    };

}