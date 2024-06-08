//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API UUID final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UUID(uint64 hash = static_cast<uint64>(std::rand()) << 32 | std::rand()); // NOTE: We use std::rand() by default, despite its limited randomness, because it is ~20x faster for serialization than std::mt19937_64

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint64 GetHash() const { return hash; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "UUID"; }
        [[nodiscard]] constexpr static Signature GetSignature() { return { 'U', 'U', 'I', 'D' }; }

        /* --- DESTRUCTORS --- */
        ~UUID() = default;

    private:
        const uint64 hash = 0;

    };

}
