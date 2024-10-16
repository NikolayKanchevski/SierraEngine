//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "../Events/Event.h"

#include "Key.h"
#include "MouseButton.h"

namespace Sierra
{

    class SIERRA_API InputEvent : public Event
    {
    public:
        /* --- COPY SEMANTICS --- */
        InputEvent(const InputEvent&) = delete;
        InputEvent& operator=(const InputEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        InputEvent(InputEvent&&) = delete;
        InputEvent& operator=(InputEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~InputEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        InputEvent() noexcept = default;

    };

    class SIERRA_API KeyEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] Key GetKey() const noexcept { return key; }

        /* --- COPY SEMANTICS --- */
        KeyEvent(const KeyEvent&) = delete;
        KeyEvent& operator=(const KeyEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        KeyEvent(InputEvent&&) = delete;
        KeyEvent& operator=(KeyEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~KeyEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit KeyEvent(Key key) noexcept;

    private:
        const Key key;

    };

    class SIERRA_API KeyPressEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit KeyPressEvent(Key key) noexcept;

        /* --- COPY SEMANTICS --- */
        KeyPressEvent(const KeyPressEvent&) = delete;
        KeyPressEvent& operator=(const KeyPressEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        KeyPressEvent(InputEvent&&) = delete;
        KeyPressEvent& operator=(KeyPressEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~KeyPressEvent() noexcept override = default;
    };

    class SIERRA_API KeyReleaseEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit KeyReleaseEvent(Key key) noexcept;

        /* --- COPY SEMANTICS --- */
        KeyReleaseEvent(const KeyReleaseEvent&) = delete;
        KeyReleaseEvent& operator=(const KeyReleaseEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        KeyReleaseEvent(InputEvent&&) = delete;
        KeyReleaseEvent& operator=(KeyReleaseEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~KeyReleaseEvent() noexcept override = default;
    };

    class SIERRA_API MouseButtonEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] MouseButton GetMouseButton() const noexcept { return mouseButton; }

        /* --- COPY SEMANTICS --- */
        MouseButtonEvent(const MouseButtonEvent&) = delete;
        MouseButtonEvent& operator=(const MouseButtonEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        MouseButtonEvent(InputEvent&&) = delete;
        MouseButtonEvent& operator=(MouseButtonEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MouseButtonEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit MouseButtonEvent(MouseButton mouseButton) noexcept;

    private:
        const MouseButton mouseButton;

    };

    class SIERRA_API MouseButtonPressEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MouseButtonPressEvent(MouseButton mouseButton) noexcept;

        /* --- COPY SEMANTICS --- */
        MouseButtonPressEvent(const MouseButtonPressEvent&) = delete;
        MouseButtonPressEvent& operator=(const MouseButtonPressEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        MouseButtonPressEvent(InputEvent&&) = delete;
        MouseButtonPressEvent& operator=(MouseButtonPressEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MouseButtonPressEvent() noexcept override = default;

    };

    class SIERRA_API MouseButtonReleaseEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MouseButtonReleaseEvent(MouseButton mouseButton) noexcept;

        /* --- COPY SEMANTICS --- */
        MouseButtonReleaseEvent(const MouseButtonReleaseEvent&) = delete;
        MouseButtonReleaseEvent& operator=(const MouseButtonReleaseEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        MouseButtonReleaseEvent(InputEvent&&) = delete;
        MouseButtonReleaseEvent& operator=(MouseButtonReleaseEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MouseButtonReleaseEvent() noexcept override = default;

    };

    class SIERRA_API MouseScrollEvent final : public InputEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MouseScrollEvent(Vector2 scroll) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] float32 GetHorizontalScroll() const noexcept { return scroll.x; }
        [[nodiscard]] float32 GetVerticalScroll() const noexcept { return scroll.y; }

        /* --- COPY SEMANTICS --- */
        MouseScrollEvent(const MouseScrollEvent&) = delete;
        MouseScrollEvent& operator=(const MouseScrollEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        MouseScrollEvent(InputEvent&&) = delete;
        MouseScrollEvent& operator=(MouseScrollEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MouseScrollEvent() noexcept override = default;

    private:
        const Vector2 scroll;

    };

}