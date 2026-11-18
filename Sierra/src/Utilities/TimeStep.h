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
        TimeStep() noexcept = default;
        explicit TimeStep(float64 duration, TimeStepUnit unit = TimeStepUnit::Seconds) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] float64 GetDuration(TimeStepUnit unit) const noexcept;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const TimeStep other) const noexcept { return duration == other.duration; }
        [[nodiscard]] bool operator!=(const TimeStep other) const noexcept { return !(*this == other); }

        [[nodiscard]] bool operator<(const TimeStep other) const noexcept { return duration < other.duration; }
        [[nodiscard]] bool operator>(const TimeStep other) const noexcept { return duration > other.duration; }
        [[nodiscard]] bool operator<=(const TimeStep other) const noexcept { return duration <= other.duration; }
        [[nodiscard]] bool operator>=(const TimeStep other) const noexcept { return duration >= other.duration; }

        [[nodiscard]] TimeStep operator+(const TimeStep other) const noexcept { return TimeStep(duration + other.duration, TimeStepUnit::Milliseconds); }
        [[nodiscard]] TimeStep operator-(const TimeStep other) const noexcept { return TimeStep(duration - other.duration, TimeStepUnit::Milliseconds); }

        [[nodiscard]] operator float32() const noexcept { return static_cast<float32>(GetDuration(TimeStepUnit::Seconds)); }

        /* --- COPY SEMANTICS --- */
        TimeStep(const TimeStep&) noexcept = default;
        TimeStep& operator=(const TimeStep&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        TimeStep(TimeStep&&) noexcept = default;
        TimeStep& operator=(TimeStep&&) noexcept = default;

    private:
        float64 duration = 0.0;

    };

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API std::string TimeStepToString(TimeStep timeStep) noexcept;

}
