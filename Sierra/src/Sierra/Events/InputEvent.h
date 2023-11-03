//
// Created by Nikolay Kanchevski on 5.10.23.
//

#pragma once

#include "Event.h"
#include "../Core/Key.h"
#include "../Core/MouseButton.h"

namespace Sierra
{

    class SIERRA_API InputEvent : public Event
    {

    };

    class SIERRA_API KeyEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Key GetKey() const { return key; }

    protected:
        inline explicit KeyEvent(const Key key) : key(key) { }

    private:
        Key key = Key::Unknown;

    };

    class SIERRA_API KeyPressEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit KeyPressEvent(const Key pressedKey) : KeyEvent(pressedKey) { }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline String ToString() const override { return (std::stringstream() << "Key Pressed - [Key Name: " << GetKeyName(GetKey()) << " | Key Code: " << static_cast<int16>(GetKey()) << "]").str(); }
        #endif

    };

    class SIERRA_API KeyReleaseEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit KeyReleaseEvent(const Key releasedKey) : KeyEvent(releasedKey) { }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline String ToString() const override { return (std::stringstream() << "Key Released - [Key Name: " << GetKeyName(GetKey()) << " | Key Code: " << static_cast<int16>(GetKey()) << "]").str(); }
        #endif

    };

    class SIERRA_API MouseButtonEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MouseButton GetMouseButton() const { return mouseButton; }

    protected:
        inline explicit MouseButtonEvent(const MouseButton mouseButton) : mouseButton(mouseButton) { }

    private:
        MouseButton mouseButton = MouseButton::Unknown;

    };

    class SIERRA_API MouseButtonPressEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseButtonPressEvent(const MouseButton mouseButton) : MouseButtonEvent(mouseButton) { }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline String ToString() const override { return (std::stringstream() << "Mouse Button Pressed - [Button Name: " << GetMouseButtonName(GetMouseButton()) << " | Button Code: " << static_cast<uint32>(GetMouseButton()) << "]").str(); }
        #endif

    };

    class SIERRA_API MouseButtonReleaseEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseButtonReleaseEvent(const MouseButton mouseButton) : MouseButtonEvent(mouseButton) { }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline String ToString() const override { return (std::stringstream() << "Mouse Button Released - [Button Name: " << GetMouseButtonName(GetMouseButton()) << " | Button Code: " << static_cast<uint32>(GetMouseButton()) << "]").str(); }
        #endif

    };

    class SIERRA_API MouseScrollEvent final : public InputEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseScrollEvent(const Vector2 scroll) : scroll(scroll) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline float32 GetHorizontalScroll() const { return scroll.x; }
        [[nodiscard]] inline float32 GetVerticalScroll() const { return scroll.y; }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline String ToString() const override { return (std::stringstream() << "Mouse Scrolled - [Horizontal Delta: " << scroll.x << " | Vertical Delta: " << scroll.y << "]").str(); }
        #endif

    private:
        Vector2 scroll;

    };

}