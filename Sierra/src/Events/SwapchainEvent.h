//
// Created by Nikolay Kanchevski on 1.01.24.
//

#pragma once

namespace Sierra
{

    class SIERRA_API SwapchainEvent : public Event
    {

    };

    class SIERRA_API SwapchainResizeEvent : public SwapchainEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit SwapchainResizeEvent(const Vector2UInt size) : size(size) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Vector2UInt GetSize() const { return size; }

    private:
        Vector2UInt size;

    };

}