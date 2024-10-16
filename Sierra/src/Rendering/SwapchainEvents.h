//
// Created by Nikolay Kanchevski on 25.09.24.
//

#pragma once

#include "../Events/Event.h"

namespace Sierra
{

    class SIERRA_API SwapchainEvent : public Event
    {
    public:
        /* --- COPY SEMANTICS --- */
        SwapchainEvent(const SwapchainEvent&) = delete;
        SwapchainEvent& operator=(const SwapchainEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        SwapchainEvent(SwapchainEvent&&) = delete;
        SwapchainEvent& operator=(SwapchainEvent&&) = delete;

        /* --- DESTRUCTORS --- */
        ~SwapchainEvent() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        SwapchainEvent() noexcept = default;

    };

    class SIERRA_API SwapchainResizeEvent final : public SwapchainEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit SwapchainResizeEvent( uint32 width, uint32 height, uint32 scaling) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept { return height; }
        [[nodiscard]] uint32 GetScaling() const noexcept { return scaling; }

        /* --- COPY SEMANTICS --- */
        SwapchainResizeEvent(const SwapchainResizeEvent&) = delete;
        SwapchainResizeEvent& operator=(const SwapchainResizeEvent&) = delete;

        /* --- MOVE SEMANTICS --- */
        SwapchainResizeEvent(SwapchainResizeEvent&&) = delete;
        SwapchainResizeEvent& operator=(SwapchainResizeEvent&&) = delete;

        /* --- DESTRUCTOR --- */
        ~SwapchainResizeEvent() noexcept override = default;

    private:
        const uint32 scaling;
        const uint32 width;
        const uint32 height;

    };

}