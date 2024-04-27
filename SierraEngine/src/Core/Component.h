//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

namespace SierraEngine
{

    class Component
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using Signature = std::array<char, 4>;

        /* --- OPERATORS --- */
        Component(const Component&) = delete;
        Component &operator=(const Component&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Component() = default;

    protected:
        Component() = default;

    };

    template<typename T>
    constexpr bool IsComponent = std::is_base_of_v<Component, T> && !std::is_same_v<Component, T> && std::is_default_constructible_v<T>;

    template<typename... Components> requires (IsComponent<Components> && ...)
    struct ComponentGroup { };

    template<typename T> requires (IsComponent<T>)
    constexpr std::string_view GetComponentName();

    template<typename T> requires (IsComponent<T>)
    constexpr Component::Signature GetComponentSignature();

}
