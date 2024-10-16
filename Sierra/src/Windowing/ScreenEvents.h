//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "../Events/Event.h"

#include "ScreenOrientation.h"

namespace Sierra
{

    class SIERRA_API ScreenEvent : public Event
    {
    public:
        /* --- COPY SEMANTICS --- */
        ScreenEvent(const ScreenEvent&) = delete;
        ScreenEvent& operator=(const ScreenEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        ScreenEvent(ScreenEvent&&) = delete;
        ScreenEvent& operator=(ScreenEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ScreenEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ScreenEvent() noexcept = default;

    };

    class SIERRA_API ScreenReorientEvent : public ScreenEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ScreenReorientEvent(ScreenOrientation orientation) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] ScreenOrientation GetOrientation() const noexcept { return orientation; }

        /* --- COPY SEMANTICS --- */
        ScreenReorientEvent(const ScreenReorientEvent&) = delete;
        ScreenReorientEvent& operator=(const ScreenReorientEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        ScreenReorientEvent(ScreenReorientEvent&&) = delete;
        ScreenReorientEvent& operator=(ScreenReorientEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ScreenReorientEvent() noexcept override = default;

    private:
        const ScreenOrientation orientation;

    };

}