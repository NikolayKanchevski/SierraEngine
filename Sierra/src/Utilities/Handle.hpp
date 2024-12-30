//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    /* --- CONCEPTS --- */
    template<typename T>
    concept HandleType = std::is_unsigned_v<T>;

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

        template<typename ValueType> requires (std::is_convertible_v<ValueType, T>)
        [[nodiscard]] bool operator==(const ValueType otherValue) const noexcept { return value == otherValue; }

        template<typename ValueType> requires (std::is_convertible_v<ValueType, T>)
        [[nodiscard]] bool operator!=(const ValueType otherValue) const noexcept { return value != otherValue; }

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

namespace std
{

    template<Sierra::HandleType T>
    struct hash<Sierra::Handle<T>>
    {
        size_t operator()(const Sierra::Handle<T> handle) const noexcept { return handle.GetValue(); }
    };

}
