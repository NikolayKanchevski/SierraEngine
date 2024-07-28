//
// Created by Nikolay Kanchevski on 14.10.23.
//

#pragma once

#include "Time.h"

namespace Sierra
{

    enum class TouchType : bool
    {
        Press,
        Release
    };

    /* --- TYPE DEFINITIONS --- */
    using TouchID = uint64;

    struct TouchCreateInfo
    {
        TouchID ID = 0;
        TouchType type = TouchType::Press;

        TimePoint tapTime = TimePoint::Now();
        float32 force = 0.0f;

        Vector2 position = { 0.0f, 0.0f };
        Vector2 lastPosition = {0.0f, 0.0f };

    };

    class SIERRA_API Touch
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Touch(const TouchCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] TouchID GetID() const { return ID; }
        [[nodiscard]] TouchType GetType() const { return type; }

        [[nodiscard]] float32 GetForce() const { return force; }
        [[nodiscard]] TimePoint GetTapTime() const { return tapTime; }
        [[nodiscard]] TimeStep GetHoldDuration() const { return TimeStep(TimePoint::Now() - tapTime); }

        [[nodiscard]] Vector2 GetPosition() const { return position; }
        [[nodiscard]] Vector2 GetLastPosition() const { return lastPosition; }
        [[nodiscard]] Vector2 GetDeltaPosition() const { return position - lastPosition; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const Touch &other) const { return ID == other.ID; }
        [[nodiscard]] bool operator!=(const Touch &other) const { return !(*this == other); }

    private:
        TouchID ID = 0;
        TouchType type = TouchType::Press;

        TimePoint tapTime = TimePoint::Now();
        float32 force = 0.0f;

        Vector2 position = { 0.0f, 0.0f };
        Vector2 lastPosition = { 0.0f, 0.0f };

    };

}
