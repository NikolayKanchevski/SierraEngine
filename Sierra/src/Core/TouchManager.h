//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "Touch.h"
#include "../Events/TouchEvent.h"

namespace Sierra
{

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
        [[nodiscard]] virtual uint32 GetTouchCount();
        [[nodiscard]] virtual std::optional<Touch> GetTouch(uint32 touchIndex);

        /* --- EVENTS --- */
        template<typename T> void OnEvent(TouchEventCallback<T>) { static_assert(std::is_base_of_v<TouchEvent, T> && !std::is_same_v<TouchEvent, T>, "Template function accepts derived touch events only!"); }

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
