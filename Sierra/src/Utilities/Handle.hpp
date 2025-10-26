//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    /* --- CONCEPTS --- */
    template<typename T>
    concept HandleType = UnsignedType<T>;

    template<HandleType T>
    class Handle
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ValueType = T;

        /* --- CONSTRUCTORS --- */
        constexpr Handle(const T value = T(0)) noexcept
            : value(value)
        {

        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] T GetValue() const noexcept { return value; }

        /* --- OPERATORS --- */
        [[nodiscard]] operator T() const noexcept { return value; }

        [[nodiscard]] bool operator==(const Handle other) const noexcept { return value == other.value; }
        [[nodiscard]] bool operator!=(const Handle other) const noexcept { return !(*this == other); }

        template<NumericType Numeric>
        [[nodiscard]] bool operator==(const Numeric other) const noexcept { return value == static_cast<T>(other); }

        template<NumericType Numeric>
        [[nodiscard]] bool operator!=(const Numeric other) const noexcept { return value != static_cast<T>(other); }

        /* --- COPY SEMANTICS --- */
        Handle(const Handle&) noexcept = default;
        Handle& operator=(const Handle&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Handle(Handle&&) noexcept = default;
        Handle& operator=(Handle&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Handle() noexcept = default;

    private:
        T value = T(0);

    };

}