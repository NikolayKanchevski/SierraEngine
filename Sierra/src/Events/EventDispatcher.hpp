//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

#include "Event.h"

#import "../Core/HandleManager.hpp"

namespace Sierra
{

    /* --- TYPE DEFINITIONS --- */
    using EventSubscriptionID = Handle<uint32>;

    /* --- CONCEPTS --- */
    template<typename T>
    concept EventType = std::is_base_of_v<Event, T> && !std::is_same_v<Event, T>;

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
            return callbacks.AddItem(Callback);
        }

        bool Unsubscribe(const EventSubscriptionID ID)
        {
            return callbacks.RemoveItem(ID);
        }

        template<typename... Args>
        void DispatchEvent(Args&&... args)
        {
            bool handled = false;
            const EventType event = EventType(std::forward<Args>(args)...);

            callbacks.ForEach([&handled, &event](const EventCallback& Callback) -> void
            {
                if (!handled && Callback(event))
                {
                    handled = true;
                }
            });
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
        HandleManager<EventSubscriptionID, EventCallback> callbacks = { };

    };

}
