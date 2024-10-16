//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "WindowingBackendType.h"

#include "TouchEvents.h"
#include "../Events/EventDispatcher.hpp"

namespace Sierra
{

    template<typename T>
    concept TouchEventType = std::is_base_of_v<TouchEvent, T> && !std::is_same_v<TouchEvent, std::decay_t<T>>;

    class SIERRA_API TouchManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<TouchEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterTouchPress(const Touch& touch) = 0;
        virtual void RegisterTouchMove(TouchID ID, Vector2 position) = 0;
        virtual void RegisterTouchRelease(TouchID ID) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::span<const Touch> GetTouches() const noexcept = 0;
        [[nodiscard]] virtual WindowingBackendType GetBackendType() const noexcept = 0;

        /* --- EVENTS --- */
        template<TouchEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<TouchEventType EventType>
        bool RemoveEventListener(EventSubscriptionID) noexcept;

        /* --- COPY SEMANTICS --- */
        TouchManager(const TouchManager&) = delete;
        TouchManager& operator=(const TouchManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        TouchManager(TouchManager&&) = delete;
        TouchManager& operator=(TouchManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~TouchManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        TouchManager() noexcept = default;

        /* --- GETTER METHODS --- */
        [[nodiscard]] EventDispatcher<TouchPressEvent>& GetTouchPressDispatcher() noexcept { return touchPressDispatcher; }
        [[nodiscard]] EventDispatcher<TouchMoveEvent>& GetTouchMoveDispatcher() noexcept { return touchMoveDispatcher; }
        [[nodiscard]] EventDispatcher<TouchReleaseEvent>& GetTouchReleaseDispatcher() noexcept { return touchReleaseDispatcher; }

    private:
        EventDispatcher<TouchPressEvent> touchPressDispatcher = { };
        EventDispatcher<TouchMoveEvent> touchMoveDispatcher = { };
        EventDispatcher<TouchReleaseEvent> touchReleaseDispatcher = { };

    };

    template<> inline EventSubscriptionID TouchManager::AddEventListener<TouchPressEvent>(const EventCallback<TouchPressEvent>& Callback) { return touchPressDispatcher.Subscribe(Callback); }
    template<> inline bool TouchManager::RemoveEventListener<TouchPressEvent>(const EventSubscriptionID ID) noexcept { return touchPressDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID TouchManager::AddEventListener<TouchMoveEvent>(const EventCallback<TouchMoveEvent>& Callback) { return touchMoveDispatcher.Subscribe(Callback); }
    template<> inline bool TouchManager::RemoveEventListener<TouchMoveEvent>(const EventSubscriptionID ID) noexcept { return touchMoveDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID TouchManager::AddEventListener<TouchReleaseEvent>(const EventCallback<TouchReleaseEvent>& Callback) { return touchReleaseDispatcher.Subscribe(Callback); }
    template<> inline bool TouchManager::RemoveEventListener<TouchReleaseEvent>(const EventSubscriptionID ID) noexcept { return touchReleaseDispatcher.Unsubscribe(ID); }

}
