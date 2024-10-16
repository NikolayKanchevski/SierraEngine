//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

namespace SierraEngine
{

    class SIERRA_ENGINE_API Component;
    template<typename T> concept ComponentType = std::is_base_of_v<Component, T> && !std::is_same_v<Component, std::decay_t<T>> && std::is_default_constructible_v<T> && requires { T::GetName(); };

    template<ComponentType...>
    struct ComponentGroup { };

    class SIERRA_ENGINE_API Component
    {
    public:
        /* --- SETTER METHODS --- */
        void SetEnabled(bool enabled) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsEnabled() const noexcept { return enabled; }

        /* --- COPY SEMANTICS --- */
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;

        /* --- DESTRUCTOR --- */
        ~Component() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        Component() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Component(Component&&) noexcept = default;
        Component& operator=(Component&&) noexcept = default;

    private:
        bool enabled = true;

    };
}
