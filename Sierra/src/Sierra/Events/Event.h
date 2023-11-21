//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

namespace Sierra
{

    class SIERRA_API Event
    {
    public:
        #if SR_ENABLE_LOGGING
            /* --- GETTER METHODS --- */
            [[nodiscard]] inline virtual std::string ToString() const = 0;
        #endif

    };

    template<typename T>
    class SIERRA_API EventDispatcher
    {
    public:
        static_assert(!std::is_same_v<Event, T> && std::is_base_of_v<Event, T>, "Event type provided to EventDispatcher must not be of the base type Event, but has to derive from it!");

        /* --- TYPE DEFINITIONS --- */
        typedef std::function<bool(const T&)> EventCallback;
        typedef uint32 EventSubscriptionID;

        /* --- CONSTRUCTORS --- */
        EventDispatcher() = default;

        /* --- POLLING METHODS --- */
        EventSubscriptionID Subscribe(EventCallback &Callback)
        {
            // Generate ID
            EventSubscriptionID ID;
            if (!freedIDs.empty())
            {
                ID = freedIDs.back();
                freedIDs.pop_back();
            }
            else
            {
                ID = totalIDs;
            }

            // Add callback
            totalIDs++;
            Callbacks.push_back(Callback);
            return ID;
        }

        bool Unsubscribe(const EventSubscriptionID ID)
        {
            // Check if ID has been registered
            if (ID >= Callbacks.size())
            {
                SR_WARNING("Trying to unsubscribe an event callback with an ID of [{0}], but it has not yet been registered! Returning...");
                return false;
            }

            // Remove callback reference
            Callbacks.erase(ID);

            // Recycle ID
            freedIDs.push_back(ID);
            return true;
        }

        template<typename... Args>
        void DispatchEvent(Args&&... args) const
        {
            // Immediately handle requested event
            T event = T(std::forward<Args>(args)...);
            for (const auto Callback : Callbacks)
            {
                // If event is handled, we break, so that older subscribers do not register it
                if (Callback(event))
                {
                    break;
                }
            }
        }

        template<typename... Args>
        void QueueEvent(Args&&... args)
        {
            queue.push_back(new T(std::forward<Args>(args))...);
        }

        void DispatchQueue()
        {
            // Handle all registered events
            for (auto event : queue)
            {
                for (const auto &Callback : Callbacks)
                {
                    // If event is handled, we break, so it does not go deeper in the event stack
                    if (Callback(*event))
                    {
                        break;
                    }
                }
                delete(event);
            }
            queue.clear();
        }


        /* --- DESTRUCTOR --- */
        ~EventDispatcher()
        {
            for (const auto event : queue)
            {
                delete(event);
            }
            queue.clear();
        }

        /* --- OPERATORS --- */
        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;

    private:
        std::deque<T*> queue;
        std::vector<EventCallback> Callbacks;

        uint32 totalIDs = 0;
        std::vector<EventSubscriptionID> freedIDs;

    };

}
