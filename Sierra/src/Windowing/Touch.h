//
// Created by Nikolay Kanchevski on 14.10.23.
//

#pragma once

#include "../Utilities/Time.h"
#include "../Utilities/Hash.hpp"

namespace Sierra
{

    using TouchID = Hash64;

    enum class TouchType : bool
    {
        Press,
        Release
    };

    struct TouchCreateInfo
    {
        TouchID ID = 0;
        TouchType type = TouchType::Press;

        TimePoint tapTime = TimePoint::Now();
        float32 force = 0.0f;

        Vector2 position = { 0.0f, 0.0f };
        Vector2 lastPosition = position;
    };

    class SIERRA_API Touch
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Touch(const TouchCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] TouchID GetID() const noexcept { return ID; }
        [[nodiscard]] TouchType GetType() const noexcept { return type; }

        [[nodiscard]] float32 GetForce() const noexcept { return force; }
        [[nodiscard]] TimePoint GetTapTime() const noexcept { return tapTime; }
        [[nodiscard]] TimeStep GetHoldDuration() const noexcept { return TimeStep(TimePoint::Now() - tapTime); }

        [[nodiscard]] Vector2 GetPosition() const noexcept { return position; }
        [[nodiscard]] Vector2 GetLastPosition() const noexcept { return lastPosition; }
        [[nodiscard]] Vector2 GetDeltaPosition() const noexcept { return position - lastPosition; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const Touch& other) const noexcept { return ID == other.ID; }
        [[nodiscard]] bool operator!=(const Touch& other) const noexcept { return !(*this == other); }

        /* --- COPY SEMANTICS --- */
        Touch(const Touch&) noexcept = default;
        Touch& operator=(const Touch&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Touch(Touch&&) noexcept = default;
        Touch& operator=(Touch&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Touch() noexcept = default;

    private:
        TouchID ID = 0;
        TouchType type = TouchType::Press;

        TimePoint tapTime = TimePoint::Now();
        float32 force = 0.0f;

        Vector2 position = { 0.0f, 0.0f };
        Vector2 lastPosition = { 0.0f, 0.0f };

    };

}
