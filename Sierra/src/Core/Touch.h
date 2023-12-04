//
// Created by Nikolay Kanchevski on 14.10.23.
//

#pragma once

#include "../Engine/Time.h"

namespace Sierra
{

    struct TouchCreateInfo
    {
        TimePoint tapTime = TimePoint::Now();
        uint32 tapCount = 0;
        float32 force = 0.0f;
        Vector2 position = { 0.0f, 0.0f };
        Vector2 deltaPosition = { 0.0f, 0.0f };
        void* ID = nullptr;
    };

    class SIERRA_API Touch
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Touch(const TouchCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetTapCount() const { return createInfo.tapCount; }
        [[nodiscard]] inline float32 GetForce() const { return createInfo.force; }
        [[nodiscard]] inline Vector2 GetPosition() const { return createInfo.position; }
        [[nodiscard]] inline Vector2 GetLastPosition() const { return createInfo.position - createInfo.deltaPosition; }
        [[nodiscard]] inline Vector2 GetDeltaPosition() const { return createInfo.deltaPosition; }
        [[nodiscard]] inline TimeStep GetHoldDuration() const { return TimeStep(TimePoint::Now() - createInfo.tapTime); }
        [[nodiscard]] inline void* GetID() const { return createInfo.ID; }

        /* --- OPERATORS --- */
        [[nodiscard]] inline bool operator ==(const Touch &other) const { return createInfo.ID == other.createInfo.ID; }
        [[nodiscard]] inline bool operator !=(const Touch &other) const { return !(*this == other); }

    private:
        TouchCreateInfo createInfo;

    };

}
