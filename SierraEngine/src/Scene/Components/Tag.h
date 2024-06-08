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
        explicit Tag(std::string_view tag = "Tag");

        /* --- SETTER METHODS --- */
        void SetTag(std::string_view tag);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTag() const { return tagMemory.data(); }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Tag"; }
        [[nodiscard]] constexpr static Signature GetSignature() { return { 'T', 'A', 'G', 'G' }; }

        /* --- DESTRUCTORS --- */
        ~Tag() = default;

    private:
        std::array<char, 32> tagMemory = { 'T', 'a', 'g' };

    };

}