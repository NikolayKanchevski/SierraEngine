//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API Tag final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Tag() = default;
        explicit Tag(std::string_view tag);

        /* --- SETTER METHODS --- */
        void SetTag(std::string_view tag);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTag() const { return tag; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Tag"; }

        /* --- DESTRUCTORS --- */
        ~Tag() = default;

    private:
        std::string tag;

    };

}