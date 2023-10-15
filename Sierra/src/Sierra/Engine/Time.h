//
// Created by Nikolay Kanchevski on 25.09.23.
//

#pragma once

namespace Sierra
{

    enum class TimeStepType : uint32
    {
        Nanoseconds = 1'000'000'000,
        Microseconds = 1'000'000,
        Milliseconds = 1'000,
        Seconds = 1
    };

    class SIERRA_API TimeStep
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimeStep() = default;
        explicit TimeStep(const float64 duration, TimeStepType type = TimeStepType::Milliseconds);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline float64 GetDurationInNanoseconds() const { return duration * 1'000'000.0; }
        [[nodiscard]] inline float64 GetDurationInMicroseconds() const { return duration * 1'000.0; }
        [[nodiscard]] inline float64 GetDurationInMilliseconds() const { return duration; }
        [[nodiscard]] inline float64 GetDurationInSeconds() const { return duration / 1'000.0; }

        /* --- OPERATORS --- */
        [[nodiscard]] inline operator float() { return duration; }
        [[nodiscard]] inline bool operator <(const TimeStep &other) const { return duration < other.duration; }
        [[nodiscard]] inline bool operator >(const TimeStep &other) const { return duration > other.duration; }
        [[nodiscard]] inline bool operator <=(const TimeStep &other) const { return duration <= other.duration; }
        [[nodiscard]] inline bool operator >=(const TimeStep &other) const { return duration >= other.duration; }
        [[nodiscard]] inline TimeStep operator +(const TimeStep &other) const { return TimeStep(duration + other.duration); }
        [[nodiscard]] inline TimeStep operator -(const TimeStep &other) const { return TimeStep(duration - other.duration); }

    private:
        float64 duration = 0.0f; // In milliseconds

    };

    class SIERRA_API TimePoint
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimePoint() = default;
        static TimePoint Now();

        /* --- OPERATORS --- */
        [[nodiscard]] const char* ToString() const;
        [[nodiscard]] TimeStep operator -(const TimePoint &other) const;
        [[nodiscard]] TimePoint operator +(const TimeStep &duration) const;
        [[nodiscard]] TimePoint operator -(const TimeStep &duration) const;

    private:
        TimePoint(const std::chrono::system_clock::time_point &timePoint);
        std::chrono::system_clock::time_point timePoint;

    };

}
