//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "Event.h"

namespace Sierra
{

    class SIERRA_API WindowEvent : public Event
    {

    };

    class SIERRA_API WindowResizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit WindowResizeEvent(const Vector2UInt size) : size(size) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Vector2UInt GetSize() const { return size; }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Window Resized - [Width: " << size.x << " | Height: " << size.y << "]");

    private:
        Vector2UInt size;

    };

    class SIERRA_API WindowCloseEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowCloseEvent() = default;

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Window Closed");
    };

    class SIERRA_API WindowMoveEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit WindowMoveEvent(const Vector2Int &position) : position(position) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] Vector2Int GetPosition() const { return position; }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Window Moved - [X: " << position.x << " | Y: " << position.y << "]");

    private:
        Vector2Int position;

    };

    class SIERRA_API WindowFocusEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit WindowFocusEvent(const bool focused) : focused(focused) { };

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsFocused() const { return focused; }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Window " << (focused ? "Focused" : "Unfocused"));

    private:
        bool focused;

    };

    class SIERRA_API WindowMinimizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowMinimizeEvent() = default;

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Window Minimized");

    };

    class SIERRA_API WindowMaximizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline WindowMaximizeEvent() = default;

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Window Maximized");

    };

}

