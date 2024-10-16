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
        Tag() noexcept = default;
        explicit Tag(std::string_view tag) noexcept;

        /* --- SETTER METHODS --- */
        void SetTag(std::string_view tag) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTag() const noexcept { return tag; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Tag"; }

        /* --- COPY SEMANTICS --- */
        Tag(const Tag&) = delete;
        Tag& operator=(const Tag&) = delete;

        /* --- MOVE SEMANTICS --- */
        Tag(Tag&&) noexcept = default;
        Tag& operator=(Tag&&) noexcept = default;

        /* --- DESTRUCTORS --- */
        ~Tag() noexcept = default;

    private:
        std::string tag;

    };

}