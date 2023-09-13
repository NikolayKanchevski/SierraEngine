//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "Event.h"

namespace Sierra
{

    BETTER_ENUM(
        MouseButton, uint8,
        NUM1,
        NUM2,
        NUM3,
        NUM4,
        NUM5,
        NUM6,
        NUM7,
        NUM8,
        LEFT,
        RIGHT,
        MIDDLE
    );

    class SIERRA_API MouseEvent : public Event
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MouseButton GetMouseButton() const { return mouseButton; }

    protected:
        inline explicit MouseEvent(const MouseButton mouseButton) : mouseButton(mouseButton) { }

    private:
        MouseButton mouseButton = MouseButton::NUM8;
    };

    class SIERRA_API MouseButtonPressedEvent final : public MouseEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseButtonPressedEvent(const MouseButton mouseButton, const uint32 repeatCount = 1) : MouseEvent(mouseButton), repeatCount(repeatCount) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetRepeatCount() const { return repeatCount; }
        [[nodiscard]] inline bool IsHeld() const { return repeatCount > 1; }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Mouse Button Pressed - [Button Name: " << GetMouseButton()._to_string() << " | Button Code: " << static_cast<uint32>(GetMouseButton()) << "]");

    private:
        uint32 repeatCount;

    };

    class SIERRA_API MouseButtonReleasedEvent final : public MouseEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseButtonReleasedEvent(const MouseButton mouseButton) : MouseEvent(mouseButton) { }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Mouse Button Released - [Button Name: " << GetMouseButton()._to_string() << " | Button Code: " << static_cast<uint32>(GetMouseButton()) << "]");

    };

}