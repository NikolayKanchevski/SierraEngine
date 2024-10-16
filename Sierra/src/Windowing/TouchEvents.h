//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "../Events/Event.h"

#include "Touch.h"

namespace Sierra
{

    class SIERRA_API TouchEvent : public Event
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] const Touch& GetTouch() const noexcept { return touch; }

        /* --- COPY SEMANTICS --- */
        TouchEvent(const TouchEvent&) = delete;
        TouchEvent& operator=(const TouchEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        TouchEvent(TouchEvent&&) = delete;
        TouchEvent& operator=(TouchEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~TouchEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit TouchEvent(const Touch& touch) noexcept;

    private:
        const Touch touch;

    };

    class SIERRA_API TouchPressEvent final : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchPressEvent(const Touch& touch) noexcept;

        /* --- COPY SEMANTICS --- */
        TouchPressEvent(const TouchPressEvent&) = delete;
        TouchPressEvent& operator=(const TouchPressEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        TouchPressEvent(TouchPressEvent&&) = delete;
        TouchPressEvent& operator=(TouchPressEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~TouchPressEvent() noexcept override = default;
    };

    class SIERRA_API TouchMoveEvent final : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchMoveEvent(const Touch& touch) noexcept;

        /* --- COPY SEMANTICS --- */
        TouchMoveEvent(const TouchMoveEvent&) = delete;
        TouchMoveEvent& operator=(const TouchMoveEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        TouchMoveEvent(TouchMoveEvent&&) = delete;
        TouchMoveEvent& operator=(TouchMoveEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~TouchMoveEvent() noexcept override = default;
    };

    class SIERRA_API TouchReleaseEvent final : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchReleaseEvent(const Touch& touch) noexcept;

        /* --- COPY SEMANTICS --- */
        TouchReleaseEvent(const TouchReleaseEvent&) = delete;
        TouchReleaseEvent& operator=(const TouchReleaseEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        TouchReleaseEvent(TouchReleaseEvent&&) = delete;
        TouchReleaseEvent& operator=(TouchReleaseEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~TouchReleaseEvent() noexcept override = default;
    };

}