//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "Touch.h"
#include "EventDispatcher.hpp"

namespace Sierra
{

    #pragma region Events
        class SIERRA_API TouchEvent : public Event
        {
        public:
            [[nodiscard]] inline const Touch& GetTouch() const { return touch; }

        protected:
            explicit TouchEvent(const Touch &touch) : touch(touch) { }

        private:
            Touch touch;

        };

        class SIERRA_API TouchBeginEvent final : public TouchEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            explicit TouchBeginEvent(const Touch &touch) : TouchEvent(touch) { }

        };

        class SIERRA_API TouchMoveEvent final : public TouchEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            explicit TouchMoveEvent(const Touch &touch) : TouchEvent(touch) { }

        };

        class SIERRA_API TouchEndEvent final : public TouchEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            explicit TouchEndEvent(const Touch &touch) : TouchEvent(touch) { }

        };
    #pragma endregion

    struct TouchManagerCreateInfo
    {

    };

    class SIERRA_API TouchManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<typename T>
        using TouchEventCallback = std::function<bool(const T&)>;
        
        /* --- CONSTRUCTORS --- */
        explicit TouchManager(const TouchManagerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetTouchCount() const;
        [[nodiscard]] virtual const Touch& GetTouch(uint32 touchIndex) const;

        /* --- EVENTS --- */
        template<typename T> requires (std::is_base_of_v<TouchEvent, T> && !std::is_same_v<TouchEvent, T>) void OnEvent(TouchEventCallback<T> Callback) { }

        /* --- DESTRUCTOR --- */
        virtual ~TouchManager() = default;

        /* --- OPERATORS --- */
        TouchManager(const TouchManager&) = delete;
        TouchManager& operator=(const TouchManager&) = delete;

    protected:
        [[nodiscard]] inline EventDispatcher<TouchBeginEvent>& GetTouchTapDispatcher() { return touchBeginDispatcher; };
        [[nodiscard]] inline EventDispatcher<TouchMoveEvent>& GetTouchMoveDispatcher() { return touchMoveDispatcher; };
        [[nodiscard]] inline EventDispatcher<TouchEndEvent>& GetTouchEndDispatcher() { return touchEndDispatcher; };

    private:
        EventDispatcher<TouchBeginEvent> touchBeginDispatcher;
        EventDispatcher<TouchMoveEvent> touchMoveDispatcher;
        EventDispatcher<TouchEndEvent> touchEndDispatcher;

    };

    template<> inline void TouchManager::OnEvent<TouchBeginEvent>(TouchEventCallback<TouchBeginEvent> Callback) { touchBeginDispatcher.Subscribe(Callback); }
    template<> inline void TouchManager::OnEvent<TouchMoveEvent>(TouchEventCallback<TouchMoveEvent> Callback) { touchMoveDispatcher.Subscribe(Callback); }
    template<> inline void TouchManager::OnEvent<TouchEndEvent>(TouchEventCallback<TouchEndEvent> Callback) { touchEndDispatcher.Subscribe(Callback); }

}
