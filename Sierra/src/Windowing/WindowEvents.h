//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "../Events/Event.h"

namespace Sierra
{

    class SIERRA_API WindowEvent : public Event
    {
    public:
        /* --- COPY SEMANTICS --- */
        WindowEvent(const WindowEvent&) = delete;
        WindowEvent& operator=(const WindowEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowEvent(WindowEvent&&) = delete;
        WindowEvent& operator=(WindowEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        WindowEvent() noexcept = default;

    };

    class SIERRA_API WindowResizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowResizeEvent(uint32 width, uint32 height) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept { return height; }

        /* --- COPY SEMANTICS --- */
        WindowResizeEvent(const WindowResizeEvent&) = delete;
        WindowResizeEvent& operator=(const WindowResizeEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowResizeEvent(WindowResizeEvent&&) = delete;
        WindowResizeEvent& operator=(WindowResizeEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowResizeEvent() noexcept override = default;

    private:
        const uint32 width;
        const uint32 height;

    };

    class SIERRA_API WindowCloseEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowCloseEvent() noexcept = default;

        /* --- COPY SEMANTICS --- */
        WindowCloseEvent(const WindowCloseEvent&) = delete;
        WindowCloseEvent& operator=(const WindowCloseEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowCloseEvent(WindowCloseEvent&&) = delete;
        WindowCloseEvent& operator=(WindowCloseEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowCloseEvent() noexcept override = default;
    };

    class SIERRA_API WindowMoveEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowMoveEvent(Vector2Int position) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] Vector2Int GetPosition() const noexcept { return position; }

        /* --- COPY SEMANTICS --- */
        WindowMoveEvent(const WindowMoveEvent&) = delete;
        WindowMoveEvent& operator=(const WindowMoveEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowMoveEvent(WindowMoveEvent&&) = delete;
        WindowMoveEvent& operator=(WindowMoveEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowMoveEvent() noexcept override = default;

    private:
        const Vector2Int position = { 0, 0 };

    };

    class SIERRA_API WindowFocusEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowFocusEvent(bool focused) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsFocused() const noexcept { return focused; }

        /* --- COPY SEMANTICS --- */
        WindowFocusEvent(const WindowFocusEvent&) = delete;
        WindowFocusEvent& operator=(const WindowFocusEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowFocusEvent(WindowFocusEvent&&) = delete;
        WindowFocusEvent& operator=(WindowFocusEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowFocusEvent() noexcept override = default;

    private:
        const bool focused;

    };

    class SIERRA_API WindowMinimizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowMinimizeEvent() noexcept = default;

        /* --- COPY SEMANTICS --- */
        WindowMinimizeEvent(const WindowMinimizeEvent&) = delete;
        WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowMinimizeEvent(WindowMinimizeEvent&&) = delete;
        WindowMinimizeEvent& operator=(WindowMinimizeEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowMinimizeEvent() noexcept override = default;
    };

    class SIERRA_API WindowMaximizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowMaximizeEvent() noexcept = default;

        /* --- COPY SEMANTICS --- */
        WindowMaximizeEvent(const WindowMaximizeEvent&) = delete;
        WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowMaximizeEvent(WindowMaximizeEvent&&) = delete;
        WindowMaximizeEvent& operator=(WindowMaximizeEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowMaximizeEvent() noexcept override = default;
    };

}