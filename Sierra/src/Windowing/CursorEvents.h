//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "../Events/Event.h"

namespace Sierra
{

    class SIERRA_API CursorEvent : public Event
    {
    public:
        /* --- COPY SEMANTICS --- */
        CursorEvent(const CursorEvent&) = delete;
        CursorEvent& operator=(const CursorEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        CursorEvent(CursorEvent&&) = delete;
        CursorEvent& operator=(CursorEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~CursorEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        CursorEvent() noexcept = default;

    };

    class SIERRA_API CursorMoveEvent final : public CursorEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CursorMoveEvent(Vector2 position) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] Vector2 GetPosition() const noexcept { return position; }

        /* --- COPY SEMANTICS --- */
        CursorMoveEvent(const CursorMoveEvent&) = delete;
        CursorMoveEvent& operator=(const CursorMoveEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        CursorMoveEvent(CursorMoveEvent&&) = delete;
        CursorMoveEvent& operator=(CursorMoveEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~CursorMoveEvent() noexcept override = default;

    private:
        const Vector2 position;

    };


}