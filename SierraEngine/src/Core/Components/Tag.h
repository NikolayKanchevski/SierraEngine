//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    class Tag final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Tag() = default;
        explicit Tag(std::string_view name);

        /* --- SETTER METHODS --- */
        inline void SetName(const std::string_view newName) { if (!newName.empty()) strncpy(name.data(), newName.data(), std::min(newName.size(), name.max_size())); }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetName() const { return name.data(); }

    private:
        std::array<char, 32> name = { 'N', 'a', 'm', 'e' };

    };

    template<> constexpr std::string_view GetComponentName<Tag>() { return "Tag"; }
    template<> constexpr Component::Signature GetComponentSignature<Tag>() { return { 'T', 'A', 'G', 'G' }; }

}