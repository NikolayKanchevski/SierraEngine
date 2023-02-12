//
// Created by Nikolay Kanchevski on 18.01.23.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine::Components
{

    class UUID : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        UUID();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetValue() const { return value; }

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

        /* --- OPERATORS --- */
        UUID& operator=(uint givenValue);

        UUID(const UUID&) = default;
        operator uint() const noexcept { return value; }

    private:
        uint64 value;

    };

}
