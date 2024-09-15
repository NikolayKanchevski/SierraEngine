//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

namespace Sierra
{
    template<typename T> concept HashType = std::is_unsigned_v<T>;

    template<HashType T>
    class UUID final
    {
    public:
        /* --- CONSTRUCTORS --- */
        UUID() = default;
        UUID(const T hash) : hash(hash) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] T GetHash() const { return hash; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const UUID other) const { return hash == other.hash; }
        [[nodiscard]] bool operator!=(const UUID other) const { return hash != other.hash; }

        /* --- COPY SEMANTICS --- */
        UUID(const UUID&) = default;
        UUID& operator=(const UUID&) = default;

        /* --- MOVE SEMANTICS --- */
        UUID(UUID&&) = default;
        UUID& operator=(UUID&&) = default;

        /* --- DESTRUCTOR --- */
        ~UUID() = default;

    private:
        T hash = 0;

    };

    /* --- TYPE DEFINITIONS --- */
    using UUID32 = UUID<uint32>;
    using UUID64 = UUID<uint64>;

}

template<Sierra::HashType T>
struct std::hash<Sierra::UUID<T>>
{
    size_t operator()(const Sierra::UUID<T> UUID) const { return UUID.GetHash(); }
};