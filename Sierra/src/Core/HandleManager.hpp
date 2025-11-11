//
// Created by Nikolay Kanchevski on 28.10.25.
//

#pragma once

#include "Handle.hpp"

namespace Sierra
{

    /* --- CONCEPTS --- */
    template<typename T>
    concept HandleType = std::is_same_v<T, Handle<typename T::ValueType>> || std::is_base_of_v<Handle<typename T::ValueType>, T>;

    template<HandleType HandleType, typename Item = void>
    class SIERRA_API HandleManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ItemEnumerationPredicate = std::function<void(Item&)>;
        using ConstItemEnumerationPredicate = std::function<void(const Item&)>;

        /* --- CONSTRUCTORS --- */
        HandleManager() noexcept = default;

        /* --- POLLING METHODS --- */

        template<typename... Args>
        [[nodiscard]] HandleType AddItem(Args&&... args)
        {
            HandleType handle;
            static_cast<void>(AddItem(handle, std::forward<Args>(args)...));
            return handle;
        }

        template<typename... Args>
        [[nodiscard]] Item& AddItem(HandleType& handle, Args&&... args)
        {
            if (!freedHandles.empty())
            {
                handle = freedHandles.back();
                freedHandles.pop_back();

                return items[handle.GetValue()].emplace(std::forward<Args>(args)...);
            }

            handle = HandleType(static_cast<HandleType::ValueType>(items.size()));
            return items.emplace_back().emplace(std::forward<Args>(args)...);
        }

        bool RemoveItem(const HandleType handle)
        {
            if (!ItemExists(handle))
            {
                return false;
            }

            items[handle.GetValue()].reset();
            freedHandles.emplace_back(handle);
            return true;
        }

        void ForEach(const ItemEnumerationPredicate& Predicate)
        {
            for (std::optional<Item>& item : items)
            {
                if (!item.has_value()) continue;
                Predicate(*item);
            }
        }

        void ForEach(const ConstItemEnumerationPredicate& Predicate) const
        {
            for (const std::optional<Item>& item : items)
            {
                if (!item.has_value()) continue;
                Predicate(*item);
            }
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool ItemExists(const HandleType handle) const noexcept
        {
            return handle.GetValue() < items.size() && items[handle.GetValue()].has_value();
        }

        [[nodiscard]] Item* GetItem(const HandleType handle) noexcept
        {
            if (!ItemExists(handle)) return nullptr;

            std::optional<Item>& item = items[handle.GetValue()];
            if (!item.has_value()) return nullptr;

            return &item.value();
        }

        [[nodiscard]] const Item* GetItem(const HandleType handle) const noexcept
        {
            if (!ItemExists(handle)) return nullptr;

            const std::optional<Item>& item = items[handle.GetValue()];
            if (!item.has_value()) return nullptr;

            return &item.value();
        }

        /* --- COPY SEMANTICS --- */
        HandleManager(const HandleManager&) = delete;
        HandleManager& operator=(const HandleManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        HandleManager(HandleManager&&) noexcept = default;
        HandleManager& operator=(HandleManager&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~HandleManager() noexcept = default;

    private:
        std::vector<std::optional<Item>> items = { };
        std::vector<HandleType> freedHandles = { };

    };

    template<HandleType HandleType>
    class HandleManager<HandleType, void>
    {
    public:
        /* --- CONSTRUCTORS --- */
        HandleManager() noexcept = default;

        /* --- POLLING METHODS --- */

        [[nodiscard]] HandleType AddItem() noexcept
        {
            if (!freedHandles.empty())
            {
                HandleType handle = freedHandles.back();
                freedHandles.pop_back();

                return handle;
            }

            const HandleType handle(static_cast<HandleType::ValueType>(currentHandle));
            ++currentHandle;

            return handle;
        }

        bool RemoveItem(const HandleType handle) noexcept
        {
            if (std::find(freedHandles.begin(), freedHandles.end(), handle) != freedHandles.end())
            {
                return false;
            }

            freedHandles.push_back(handle);
            return true;
        }

        [[nodiscard]] bool ItemExists(const HandleType handle) const noexcept
        {
            return handle.GetValue() < currentHandle && std::find(freedHandles.begin(), freedHandles.end(), handle) == freedHandles.end();
        }

        /* --- COPY SEMANTICS --- */
        HandleManager(const HandleManager&) = delete;
        HandleManager& operator=(const HandleManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        HandleManager(HandleManager&&) noexcept = default;
        HandleManager& operator=(HandleManager&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~HandleManager() noexcept = default;

    private:
        HandleType::ValueType currentHandle = 0;
        std::vector<HandleType> freedHandles = { };


    };


}