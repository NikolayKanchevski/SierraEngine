//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

namespace SierraEngine
{

    class Component;
    template<typename T> concept ComponentType = std::is_base_of_v<Component, T> && !std::is_same_v<Component, std::decay_t<T>> && requires { T::GetName(); } && requires { T::GetSignature(); };

    template<ComponentType...>
    struct ComponentGroup { };

    class SIERRA_ENGINE_API Component
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using Signature = std::array<char, 4>;

        /* --- SETTER METHODS --- */
        void SetEnabled(bool enabled);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsEnabled() const { return enabled; }

        /* --- OPERATORS --- */
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;

    protected:
        Component() = default;

    private:
        bool enabled = true;

    };
}
