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
        Tag(String givenTag) : tag(std::move(givenTag)) { }

        /* --- PROPERTIES --- */
        String tag;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

        /* --- OPERATORS --- */
        Tag() = default;
        Tag(const Tag&) = default;
    };
}