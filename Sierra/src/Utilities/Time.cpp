//
// Created by Nikolay Kanchevski on 25.09.23.
//

#include "Time.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    TimeStep::TimeStep(const float64 duration, const TimeStepType type)
    {
        switch (type)
        {
            case TimeStepType::Nanoseconds:
            {
                this->duration = duration * 0.000'001;
                break;
            }
            case TimeStepType::Microseconds:
            {
                this->duration = duration * 0.001;
                break;
            }
            case TimeStepType::Milliseconds:
            {
                this->duration = duration * 1.0;
                break;
            }
            case TimeStepType::Seconds:
            {
                this->duration = duration * 1'000;
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

    TimePoint TimePoint::operator-(const TimeStep &duration) const
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint - std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(duration.GetDurationInMilliseconds())))));
    }

    TimePoint TimePoint::operator+(const TimeStep &duration) const
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(duration.GetDurationInMilliseconds())))));
    }

    TimeStep TimePoint::operator-(const TimePoint &other) const
    {
        return TimeStep(static_cast<float64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float64>(timePoint - other.timePoint)).count()));
    }

}
