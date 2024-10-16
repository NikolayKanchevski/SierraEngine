//
// Created by Nikolay Kanchevski on 25.09.23.
//

#include "Time.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    TimeStep::TimeStep(const float64 givenDuration, const TimeStepUnit unit) noexcept
    {
        switch (unit)
        {
            case TimeStepUnit::Nanoseconds:
            {
                duration = givenDuration * 0.000'001;
                break;
            }
            case TimeStepUnit::Microseconds:
            {
                duration = givenDuration * 0.001;
                break;
            }
            case TimeStepUnit::Milliseconds:
            {
                duration = givenDuration * 1.0;
                break;
            }
            case TimeStepUnit::Seconds:
            {
                duration = givenDuration * 1'000;
                break;
            }
        }
    }

    /* --- CONSTRUCTORS --- */

    TimePoint::TimePoint(const std::chrono::system_clock::time_point timePoint) noexcept
        : timePoint(timePoint)
    {

    }

    TimePoint TimePoint::Now() noexcept
    {
        return TimePoint(std::chrono::system_clock::now());
    }

    /* --- OPERATORS --- */

    TimePoint TimePoint::operator-(const TimeStep step) const noexcept
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint - std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(step.GetDurationInMilliseconds())))));
    }

    TimePoint TimePoint::operator+(const TimeStep step) const noexcept
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(step.GetDurationInMilliseconds())))));
    }

    TimeStep TimePoint::operator-(const TimePoint other) const noexcept
    {
        return TimeStep(static_cast<float64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float64>(timePoint - other.timePoint)).count()));
    }

}
