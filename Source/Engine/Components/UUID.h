//
// Created by Nikolay Kanchevski on 18.01.23.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine
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
        inline UUID& operator=(const uint32 givenValue) { value = givenValue; return *this; };
        inline operator uint() const { return value; }

    private:
        uint64 value;

    };

}
