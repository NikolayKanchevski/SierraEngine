//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

namespace Sierra
{

    class SIERRA_API Event {  };

    template<typename T> requires (!std::is_same_v<Event, T> && std::is_base_of_v<Event, T>)
    class SIERRA_API EventDispatcher final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using EventCallback = std::function<bool(const T&)>;
        using EventSubscriptionID = uint32;

        /* --- CONSTRUCTORS --- */
        EventDispatcher() = default;

        /* --- POLLING METHODS --- */
        EventSubscriptionID Subscribe(const EventCallback &Callback)
        {
            // Generate ID
            EventSubscriptionID ID;
            if (!freedIDs.empty())
            {
                ID = freedIDs.front();
                freedIDs.pop();
            }
            else
            {
                ID = totalIDs;
            }

            // Add callback
            totalIDs++;
            callbacks.push_back(Callback);
            return ID;
        }

        bool Unsubscribe(const EventSubscriptionID ID)
        {
            // Check if ID has been registered
            if (ID >= callbacks.size()) return false;

            // Remove callback reference
            callbacks.erase(ID);

            // Recycle ID
            freedIDs.push(ID);
            return true;
        }

        template<typename... Args>
        void DispatchEvent(Args&&... args)
        {
            // Immediately handle requested event
            T event = T(std::forward<Args>(args)...);
            for (const EventCallback &Callback : callbacks)
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
        std::deque<EventCallback> callbacks;

        uint32 totalIDs = 0;
        std::queue<EventSubscriptionID> freedIDs;

    };

}
