//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

namespace Sierra
{

    using EventSubscriptionID = uint32;
    class SIERRA_API Event { protected: Event() = default; };
    template<typename T> concept EventType = !std::is_same_v<Event, T> && std::is_base_of_v<Event, T>;

    template<EventType EventType>
    class SIERRA_API EventDispatcher final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- CONSTRUCTORS --- */
        EventDispatcher() = default;

        /* --- POLLING METHODS --- */
        EventSubscriptionID Subscribe(const EventCallback &Callback)
        {
            // Generate ID
            EventSubscriptionID ID = static_cast<EventSubscriptionID>(std::chrono::high_resolution_clock::now().time_since_epoch().count());

            // Add callback
            callbacks[ID] = Callback;
            return ID;
        }

        bool Unsubscribe(const EventSubscriptionID ID)
        {
            // Check if ID has been registered
            auto iterator = callbacks.find(ID);
            if (iterator == callbacks.end()) return false;

            // Remove callback
            callbacks.erase(iterator);

            return true;
        }

        template<typename... Args>
        void DispatchEvent(Args&&... args)
        {
            // Immediately handle requested event
            EventType event = EventType(std::forward<Args>(args)...);
            for (const auto &[ID, Callback] : callbacks)
            {
                // If event is handled, we break, so that deeper subscribers do not register it
                if (Callback(event))
                {
                    break;
                }
            }
        }

        /* --- OPERATORS --- */
        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;

        /* --- DESTRUCTOR --- */
        ~EventDispatcher() = default;

    private:
        std::unordered_map<EventSubscriptionID, EventCallback> callbacks;

    };

}
