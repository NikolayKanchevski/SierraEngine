//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    /* --- CONCEPTS --- */
    template<typename T>
    concept HandleValueType = UnsignedType<T>;

    template<HandleValueType T, T NullValue = std::numeric_limits<T>::max()>
    class Handle
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ValueType = T;

        /* --- CONSTRUCTORS --- */
        constexpr Handle() noexcept = default;

        constexpr Handle(const T value) noexcept
            : value(value)
        {

        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool HasValue() const noexcept { return value != NullValue; }
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
        T value = NullValue;

    };

}