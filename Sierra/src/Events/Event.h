//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    class SIERRA_API Event
    {
    public:
        /* --- COPY SEMANTICS --- */
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;

        /* --- MOVE SEMANTICS --- */
        Event(Event&&) = delete;
        Event& operator=(Event&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Event() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        Event() noexcept = default;

    };

}