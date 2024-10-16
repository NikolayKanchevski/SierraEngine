//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

#include "Event.h"

#include "../Utilities/RNG.h"

namespace Sierra
{

    using EventSubscriptionID = uint32;

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
        EventSubscriptionID Subscribe(const EventCallback& Callback)
        {
            // Generate ID and assign callback
            const EventSubscriptionID ID = RNG().Random<EventSubscriptionID>();
            callbacks[ID] = Callback;
            return ID;
        }

        bool Unsubscribe(const EventSubscriptionID ID) noexcept
        {
            // Check if ID has been registered
            const auto iterator = callbacks.find(ID);
            if (iterator == callbacks.end()) return false;

            // Remove callback
            callbacks.erase(iterator);
            return true;
        }

        template<typename... Args>
        void DispatchEvent(Args&&... args)
        {
            // Immediately handle requested event
            const EventType event = EventType(std::forward<Args>(args)...);
            for (const auto& [ID, Callback] : callbacks)
            {
                // If event is handled, we break, so that deeper subscribers do not register it
                if (Callback(event))
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
        std::unordered_map<EventSubscriptionID, EventCallback> callbacks = { };

    };

}
