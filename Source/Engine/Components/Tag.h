//
// Created by Nikolay Kanchevski on 18.01.23.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine
{
    class Tag : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Tag() = default;
        Tag(const String &givenTag) : tag(givenTag) { }

        /* --- PROPERTIES --- */
        String tag;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

    };
}