//
// Created by Nikolay Kanchevski on 30.10.24.
//

#pragma once

namespace Sierra
{

    /* --- CONCEPTS --- */
    template<typename T>
    concept IndexType = std::is_convertible_v<T, size> && std::is_constructible_v<T, size>;

    template<IndexType T>
    class IndexPool final
    {
    public:
        /* --- CONSTRUCTORS --- */
        IndexPool() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] T GenerateIndex() noexcept
        {
            if (!freedIndices.empty())
            {
                const size index = static_cast<size>(freedIndices.back());

                freedIndices.pop_back();
                return T(index);
            }

            const T index = currentIndex;
            currentIndex = T(static_cast<size>(currentIndex) + 1);

            return index;
        }

        bool FreeIndex(const T index)
        {
            const auto iterator = std::find(freedIndices.begin(), freedIndices.end(), index);
            if (iterator != freedIndices.end()) return false;

            freedIndices.emplace_back(index);
            return true;
        }

        /* --- COPY SEMANTICS --- */
        IndexPool(const IndexPool&) = delete;
        IndexPool& operator=(const IndexPool&) = delete;

        /* --- MOVE SEMANTICS --- */
        IndexPool(IndexPool&&) noexcept = default;
        IndexPool& operator=(IndexPool&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~IndexPool() noexcept = default;

    private:
        T currentIndex = T(0);
        std::vector<T> freedIndices = { };

    };

}