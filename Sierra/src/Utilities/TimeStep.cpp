//
// Created by Nikolay Kanchevski on 25.09.23.
//

#include "TimeStep.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    std::string TimeStepToString(const TimeStep timeStep) noexcept
    {
        struct Unit
        {
            TimeStepUnit unit = TimeStepUnit::Milliseconds;
            std::string_view suffix = { };
        };

        constexpr std::array UNITS
        {
            Unit { .unit = TimeStepUnit::Years,         .suffix = "y" },
            Unit { .unit = TimeStepUnit::Months,        .suffix = "mo" },
            Unit { .unit = TimeStepUnit::Weeks,         .suffix = "w" },
            Unit { .unit = TimeStepUnit::Days,          .suffix = "d" },
            Unit { .unit = TimeStepUnit::Hours,         .suffix = "h" },
            Unit { .unit = TimeStepUnit::Minutes,       .suffix = "min" },
            Unit { .unit = TimeStepUnit::Seconds,       .suffix = "s" },
            Unit { .unit = TimeStepUnit::Milliseconds,  .suffix = "ms" },
            Unit { .unit = TimeStepUnit::Microseconds,  .suffix = "us" }
        };

        // Use the largest unit in which the step is at least one
        for (const Unit& unit : UNITS)
        {
            const float64 duration = timeStep.GetDuration(unit.unit);
            if (std::abs(duration) >= 1.0) return SR_FORMAT("{0:.2f} {1}", duration, unit.suffix);
        }

        return SR_FORMAT("{0:.2f} ns", timeStep.GetDuration(TimeStepUnit::Nanoseconds));
    }

}
