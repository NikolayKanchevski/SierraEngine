//
// Created by Nikolay Kanchevski on 25.09.23.
//

#include "TimeStep.h"

namespace Sierra
{

    namespace
    {
        constexpr std::array<std::string_view, 10> SUFFIXES
        {
            "ns",
            "us",
            "ms",
            "s" ,
            "min",
            "h" ,
            "d" ,
            "w" ,
            "mo",
            "y"
        };

        float64 UnitToDuration(const TimeStepUnit unit) noexcept
        {
            switch (unit)
            {
                case TimeStepUnit::Nanoseconds:     return 0.000'001;
                case TimeStepUnit::Microseconds:    return 0.001;
                case TimeStepUnit::Milliseconds:    return 1.0;
                case TimeStepUnit::Seconds:         return 1'000.0;
                case TimeStepUnit::Minutes:         return 60'000.0;
                case TimeStepUnit::Hours:           return 3'600'000.0;
                case TimeStepUnit::Days:            return 86'400'000.0;
                case TimeStepUnit::Weeks:           return 604'800'000.0;
                case TimeStepUnit::Months:          return 2'629'746'000.0;
                case TimeStepUnit::Years:           return 31'556'952'000.0;
            }

            return 1.0;
        }
    }

    /* --- CONSTRUCTORS --- */

    TimeStep::TimeStep(const float64 givenDuration, const TimeStepUnit unit) noexcept
        : duration(givenDuration * UnitToDuration(unit))
    {

    }

    /* --- GETTER METHODS --- */

    float64 TimeStep::GetDuration(const TimeStepUnit unit) const noexcept
    {
        return duration / UnitToDuration(unit);
    }

    /* --- CONVERSIONS --- */

    std::string TimeStepToString(const TimeStep timeStep) noexcept
    {
        for (size i = SUFFIXES.size(); i-- > 0;)
        {
            const TimeStepUnit unit = static_cast<TimeStepUnit>(i);

            const float64 duration = timeStep.GetDuration(unit);
            if (glm::abs(duration) >= 1.0) return SR_FORMAT("{0:.2f} {1}", duration, SUFFIXES[i]);
        }

        return "Unknown";
    }

}
