//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    template<typename T>
    concept HashType = std::is_unsigned_v<T>;

    template<HashType T>
    class Hash final
    {
    public:
        /* --- CONSTRUCTORS --- */
        Hash(const T value = 0) noexcept
            : value(value)
        {

        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] T GetValue() const noexcept { return value; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const Hash other) const noexcept { return value == other.value; }
        [[nodiscard]] bool operator!=(const Hash other) const noexcept { return !(*this == other); }

        /* --- COPY SEMANTICS --- */
        Hash(const Hash&) noexcept = default;
        Hash& operator=(const Hash&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Hash(Hash&&) noexcept = default;
        Hash& operator=(Hash&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Hash() noexcept = default;

    private:
        T value = { };

    };

    using Hash32 = Hash<uint32>;
    using Hash64 = Hash<uint64>;

}

template<Sierra::HashType T>
struct std::hash<Sierra::Hash<T>>
{
    size_t operator()(const Sierra::Hash<T> hash) const noexcept { return hash.GetValue(); }
};