//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    class UUID final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        UUID();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetHash() const { return hash; }

    private:
        const uint64 hash = 0;

    };

    template<> constexpr std::string_view GetComponentName<UUID>() { return "UUID"; }
    template<> constexpr Component::Signature GetComponentSignature<UUID>() { return { 'U', 'U', 'I', 'D' }; }

}
