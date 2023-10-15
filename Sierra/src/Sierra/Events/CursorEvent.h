//
// Created by Nikolay Kanchevski on 5.10.23.
//

#pragma once

#include "Event.h"

namespace Sierra
{


    class SIERRA_API CursorEvent : public Event
    {
    protected:
        CursorEvent() = default;

    };

    class SIERRA_API CursorMoveEvent final : public CursorEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit CursorMoveEvent(const Vector2 &position) : position(position) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const Vector2& GetPosition() const { return position; }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline String ToString() const override { return (std::stringstream() << "Cursor Moved - [X: " << position.x << " | Y: " << position.y << "]").str(); }
        #endif

    private:
        Vector2 position;

    };

}