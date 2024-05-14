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
        void SetName(std::string_view tag);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetTag() const { return tag.data(); }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Tag"; }
        [[nodiscard]] constexpr static Signature GetSignature() { return { 'T', 'A', 'G', 'G' }; }

        /* --- DESTRUCTORS --- */
        ~Tag() = default;

    private:
        std::array<char, 32> tag = { 'T', 'a', 'g' };

    };

}