//
// Created by Nikolay Kanchevski on 25.09.23.
//

#include "Time.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    TimeStep::TimeStep(const float64 givenDuration, const TimeStepType type)
    {
        switch (type)
        {
            case TimeStepType::Nanoseconds:
            {
                duration = givenDuration * 0.000'001;
                break;
            }
            case TimeStepType::Microseconds:
            {
                duration = givenDuration * 0.001;
                break;
            }
            case TimeStepType::Milliseconds:
            {
                duration = givenDuration * 1.0;
                break;
            }
            case TimeStepType::Seconds:
            {
                duration = givenDuration * 1'000;
                break;
            }
        }
    }

    /* --- CONSTRUCTORS --- */

    TimePoint::TimePoint(const std::chrono::system_clock::time_point &timePoint)
        : timePoint(timePoint)
    {

    }

    TimePoint TimePoint::Now()
    {
        return TimePoint(std::chrono::system_clock::now());
    }

    /* --- OPERATORS --- */

    String TimePoint::ToString() const
    {
        const std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
        return (std::stringstream() << time).str();
    }

    TimeStep TimePoint::operator-(const Sierra::TimePoint &other) const
    {
        return TimeStep(static_cast<float64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float64>(timePoint - other.timePoint)).count()));
    }

    TimePoint TimePoint::operator-(const TimeStep &duration) const
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint - std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(duration.GetDurationInMilliseconds())))));
    }

    TimePoint TimePoint::operator+(const TimeStep &duration) const
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(duration.GetDurationInMilliseconds())))));
    }

}
