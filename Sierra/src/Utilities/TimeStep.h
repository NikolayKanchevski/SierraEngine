//
// Created by Nikolay Kanchevski on 25.09.23.
//

#pragma once

namespace Sierra
{

    enum class TimeStepUnit : uint8
    {
        Nanoseconds,
        Microseconds,
        Milliseconds,
        Seconds,
        Minutes,
        Hours,
        Days,
        Weeks,
        Months,
        Years
    };

    class SIERRA_API TimeStep final
    {
    public:
        /* --- CONSTRUCTORS --- */
        constexpr TimeStep() noexcept = default;
        constexpr explicit TimeStep(const float64 givenDuration, const TimeStepUnit unit = TimeStepUnit::Milliseconds) noexcept : duration(givenDuration * GetUnitDuration(unit)) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] constexpr float64 GetDuration(const TimeStepUnit unit) const noexcept { return duration / GetUnitDuration(unit); }

        /* --- OPERATORS --- */
        [[nodiscard]] constexpr bool operator==(const TimeStep other) const noexcept { return duration == other.duration; }
        [[nodiscard]] constexpr bool operator!=(const TimeStep other) const noexcept { return !(*this == other); }

        [[nodiscard]] constexpr bool operator<(const TimeStep other) const noexcept { return duration < other.duration; }
        [[nodiscard]] constexpr bool operator>(const TimeStep other) const noexcept { return duration > other.duration; }
        [[nodiscard]] constexpr bool operator<=(const TimeStep other) const noexcept { return duration <= other.duration; }
        [[nodiscard]] constexpr bool operator>=(const TimeStep other) const noexcept { return duration >= other.duration; }

        [[nodiscard]] constexpr TimeStep operator+(const TimeStep other) const noexcept { return TimeStep(duration + other.duration); }
        [[nodiscard]] constexpr TimeStep operator-(const TimeStep other) const noexcept { return TimeStep(duration - other.duration); }

        /* --- COPY SEMANTICS --- */
        constexpr TimeStep(const TimeStep&) noexcept = default;
        constexpr TimeStep& operator=(const TimeStep&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        constexpr TimeStep(TimeStep&&) noexcept = default;
        constexpr TimeStep& operator=(TimeStep&&) noexcept = default;

    private:
        float64 duration = 0.0;

        // Length of one unit in milliseconds, which is what the duration is stored in. Months and years use their average lengths
        [[nodiscard]] static constexpr float64 GetUnitDuration(const TimeStepUnit unit) noexcept
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

    };

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API std::string TimeStepToString(TimeStep timeStep) noexcept;

}
