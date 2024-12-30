//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

#include "Event.h"

#include "../Utilities/Handle.hpp"
#include "../Utilities/IndexPool.hpp"

namespace Sierra
{

    /* --- TYPE DEFINITIONS --- */
    using EventSubscriptionID = Handle<uint32>;

    /* --- CONCEPTS --- */
    template<typename T>
    concept EventType = !std::is_same_v<Event, T> && std::is_base_of_v<Event, T>;

    template<EventType EventType>
    class SIERRA_API EventDispatcher final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- CONSTRUCTORS --- */
        EventDispatcher() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] EventSubscriptionID Subscribe(const EventCallback& Callback)
        {
            const EventSubscriptionID ID = indexPool.GenerateIndex();

            if (ID >= callbacks.size()) callbacks.emplace_back(Callback);
            else callbacks[ID].emplace(Callback);

            return ID;
        }

        bool Unsubscribe(const EventSubscriptionID ID)
        {
            if (ID >= callbacks.size() || !callbacks[ID].has_value())
            {
                return false;
            }

            callbacks[ID] = std::nullopt;
            indexPool.FreeIndex(ID);

            return true;
        }

        template<typename... Args>
        void DispatchEvent(Args&&... args)
        {
            const EventType event = EventType(std::forward<Args>(args)...);
            for (const std::optional<EventCallback>& Callback : callbacks)
            {
                // If event is handled, we break, so that early subscribers do not register it
                if (Callback.has_value() && Callback.value()(event))
                {
                    break;
                }
            }
        }

        /* --- COPY SEMANTICS --- */
        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;

        /* --- MOVE SEMANTICS --- */
        EventDispatcher(EventDispatcher&&) noexcept = default;
        EventDispatcher& operator=(EventDispatcher&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~EventDispatcher() noexcept = default;

    private:
        IndexPool<EventSubscriptionID> indexPool;
        std::vector<std::optional<EventCallback>> callbacks = { };

    };

}
