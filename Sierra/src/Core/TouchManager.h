//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "Touch.h"
#include "EventDispatcher.hpp"

namespace Sierra
{

    class SIERRA_API TouchEvent;
    template<typename T> concept TouchEventType = std::is_base_of_v<TouchEvent, T> && !std::is_same_v<TouchEvent, std::decay_t<T>>;

    class SIERRA_API TouchEvent : public Event
    {
    public:
        [[nodiscard]] const Touch& GetTouch() const { return touch; }

    protected:
        explicit TouchEvent(const Touch &touch) : touch(touch) { }

    private:
        Touch touch;

    };

    class SIERRA_API TouchPressEvent final : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchPressEvent(const Touch &touch) : TouchEvent(touch) { }

    };

    class SIERRA_API TouchMoveEvent final : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchMoveEvent(const Touch &touch) : TouchEvent(touch) { }

    };

    class SIERRA_API TouchReleaseEvent final : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchReleaseEvent(const Touch &touch) : TouchEvent(touch) { }

    };

    class SIERRA_API TouchManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<TouchEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterTouchPress(const Touch &touch);
        virtual void RegisterTouchMove(TouchID ID, Vector2 position);
        virtual void RegisterTouchRelease(TouchID ID);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::span<const Touch> GetTouches() const;

        /* --- EVENTS --- */
        template<TouchEventType EventType>
        EventSubscriptionID AddEventListener(EventCallback<EventType>);

        template<TouchEventType EventType>
        bool RemoveEventListener(EventSubscriptionID);

        /* --- DESTRUCTOR --- */
        virtual ~TouchManager() = default;

        /* --- OPERATORS --- */
        TouchManager(const TouchManager&) = delete;
        TouchManager& operator=(const TouchManager&) = delete;

    protected:
        TouchManager() = default;

        [[nodiscard]] EventDispatcher<TouchPressEvent>& GetTouchPressDispatcher() { return touchPressDispatcher; }
        [[nodiscard]] EventDispatcher<TouchMoveEvent>& GetTouchMoveDispatcher() { return touchMoveDispatcher; }
        [[nodiscard]] EventDispatcher<TouchReleaseEvent>& GetTouchReleaseDispatcher() { return touchReleaseDispatcher; }

    private:
        EventDispatcher<TouchPressEvent> touchPressDispatcher;
        EventDispatcher<TouchMoveEvent> touchMoveDispatcher;
        EventDispatcher<TouchReleaseEvent> touchReleaseDispatcher;

    };

    template<> inline EventSubscriptionID TouchManager::AddEventListener<TouchPressEvent>(EventCallback<TouchPressEvent> Callback) { return touchPressDispatcher.Subscribe(Callback); }
    template<> inline bool TouchManager::RemoveEventListener<TouchPressEvent>(const EventSubscriptionID ID) { return touchPressDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID TouchManager::AddEventListener<TouchMoveEvent>(EventCallback<TouchMoveEvent> Callback) { return touchMoveDispatcher.Subscribe(Callback); }
    template<> inline bool TouchManager::RemoveEventListener<TouchMoveEvent>(const EventSubscriptionID ID) { return touchMoveDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID TouchManager::AddEventListener<TouchReleaseEvent>(EventCallback<TouchReleaseEvent> Callback) { return touchReleaseDispatcher.Subscribe(Callback); }
    template<> inline bool TouchManager::RemoveEventListener<TouchReleaseEvent>(const EventSubscriptionID ID) { return touchReleaseDispatcher.Unsubscribe(ID); }


}
