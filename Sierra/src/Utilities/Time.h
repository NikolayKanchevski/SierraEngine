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
        Seconds
    };

    class SIERRA_API TimeStep final
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimeStep() noexcept = default;
        explicit TimeStep(float64 givenDuration, TimeStepUnit unit = TimeStepUnit::Milliseconds) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] float64 GetDurationInNanoseconds() const noexcept { return duration * 1'000'000.0; }
        [[nodiscard]] float64 GetDurationInMicroseconds() const noexcept { return duration * 1'000.0; }
        [[nodiscard]] float64 GetDurationInMilliseconds() const noexcept { return duration; }
        [[nodiscard]] float64 GetDurationInSeconds() const noexcept { return duration / 1'000.0; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const TimeStep other) const noexcept { return duration == other.duration; }
        [[nodiscard]] bool operator!=(const TimeStep other) const noexcept { return !(*this == other); }

        [[nodiscard]] bool operator<(const TimeStep other) const noexcept { return duration < other.duration; }
        [[nodiscard]] bool operator>(const TimeStep other) const noexcept { return duration > other.duration; }
        [[nodiscard]] bool operator<=(const TimeStep other) const noexcept { return duration <= other.duration; }
        [[nodiscard]] bool operator>=(const TimeStep other) const noexcept { return duration >= other.duration; }

        [[nodiscard]] TimeStep operator+(const TimeStep other) const noexcept { return TimeStep(duration + other.duration); }
        [[nodiscard]] TimeStep operator-(const TimeStep other) const noexcept { return TimeStep(duration - other.duration); }

        /* --- COPY SEMANTICS --- */
        TimeStep(const TimeStep&) noexcept = default;
        TimeStep& operator=(const TimeStep&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        TimeStep(TimeStep&&) noexcept = default;
        TimeStep& operator=(TimeStep&&) noexcept = default;

    private:
        float64 duration = 0.0f;

    };

    class SIERRA_API TimePoint final
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimePoint() noexcept = default;

        explicit TimePoint(std::chrono::system_clock::time_point timePoint) noexcept;
        static TimePoint Now() noexcept;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const TimePoint other) const noexcept { return timePoint == other.timePoint; }
        [[nodiscard]] bool operator!=(const TimePoint other) const noexcept { return !(*this == other); }

        [[nodiscard]] TimePoint operator+(TimeStep step) const noexcept;
        [[nodiscard]] TimePoint operator-(TimeStep step) const noexcept;
        [[nodiscard]] TimeStep operator-(TimePoint other) const noexcept;

        /* --- COPY SEMANTICS --- */
        TimePoint(const TimePoint&) noexcept = default;
        TimePoint& operator=(const TimePoint&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        TimePoint(TimePoint&&) noexcept = default;
        TimePoint& operator=(TimePoint&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~TimePoint() noexcept = default;

    private:
        friend class Date;
        std::chrono::system_clock::time_point timePoint;

    };

}
