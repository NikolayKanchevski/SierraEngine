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
        TimeStep() = default;
        explicit TimeStep(float64 givenDuration, TimeStepUnit unit = TimeStepUnit::Milliseconds);

        /* --- GETTER METHODS --- */
        [[nodiscard]] float64 GetDurationInNanoseconds() const { return duration * 1'000'000.0; }
        [[nodiscard]] float64 GetDurationInMicroseconds() const { return duration * 1'000.0; }
        [[nodiscard]] float64 GetDurationInMilliseconds() const { return duration; }
        [[nodiscard]] float64 GetDurationInSeconds() const { return duration / 1'000.0; }

        /* --- OPERATORS --- */
        TimeStep(const TimeStep&) = default;
        TimeStep& operator=(const TimeStep&) = default;

        [[nodiscard]] bool operator==(const TimeStep other) const { return duration == other.duration; }
        [[nodiscard]] bool operator!=(const TimeStep other) const { return !(*this == other); }

        [[nodiscard]] bool operator<(const TimeStep other) const { return duration < other.duration; }
        [[nodiscard]] bool operator>(const TimeStep other) const { return duration > other.duration; }
        [[nodiscard]] bool operator<=(const TimeStep other) const { return duration <= other.duration; }
        [[nodiscard]] bool operator>=(const TimeStep other) const { return duration >= other.duration; }

        [[nodiscard]] TimeStep operator+(const TimeStep other) const { return TimeStep(duration + other.duration); }
        [[nodiscard]] TimeStep operator-(const TimeStep other) const { return TimeStep(duration - other.duration); }

    private:
        float64 duration = 0.0f;

    };

    class SIERRA_API TimePoint final
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimePoint() = default;
        explicit TimePoint(std::chrono::system_clock::time_point timePoint);
        static TimePoint Now();

        /* --- OPERATORS --- */
        TimePoint(const TimePoint&) = default;
        TimePoint& operator=(const TimePoint&) = default;

        [[nodiscard]] bool operator==(const TimePoint other) const { return timePoint == other.timePoint; }
        [[nodiscard]] bool operator!=(const TimePoint other) const { return !(*this == other); }

        [[nodiscard]] TimePoint operator+(TimeStep step) const;
        [[nodiscard]] TimePoint operator-(TimeStep step) const;
        [[nodiscard]] TimeStep operator-(TimePoint other) const;

    private:
        friend class Date;
        std::chrono::system_clock::time_point timePoint;

    };

    enum class Day : uint8
    {
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday
    };

    enum class Month : uint8
    {
        January,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    struct DateCreateInfo
    {
        uint8 second = 0; // [0, 60]
        uint8 minute = 0; // [0, 59]
        uint8 hour = 0;   // [0, 23]
        Month month = Month::January;
        uint8 day = 1;    // [1, 31]
        uint16 year = 0;
    };

    class SIERRA_API Date final
    {
    public:
        /* --- CONSTRUCTORS --- */
        Date() = default;
        explicit Date(const DateCreateInfo &createInfo);
        explicit Date(const TimePoint &point);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint8 GetSecond() const { return second; }
        [[nodiscard]] uint8 GetMinute() const { return minute; }
        [[nodiscard]] uint8 GetHour() const { return hour; }
        [[nodiscard]] Month GetMonth() const { return month; }
        [[nodiscard]] uint8 GetDay() const { return day; }
        [[nodiscard]] uint16 GetYear() const { return year; }

        [[nodiscard]] Day GetWeekDay() const { return weekDay; }
        [[nodiscard]] uint16 GetYearDay() const { return yearDay; }

        /* --- OPERATORS --- */
        Date(const Date&) = default;
        Date& operator=(const Date&) = default;

        [[nodiscard]] bool operator==(Date other) const;
        [[nodiscard]] bool operator!=(Date other) const;

        [[nodiscard]] bool operator<(Date other) const;
        [[nodiscard]] bool operator>(Date other) const;
        [[nodiscard]] bool operator<=(Date other) const;
        [[nodiscard]] bool operator>=(Date other) const;

        /* --- DESTRUCTORS --- */
        ~Date() = default;

    private:
        uint8 second = 0; // [0, 60]
        uint8 minute = 0; // [0, 59]
        uint8 hour = 0;   // [0, 23]
        Month month = Month::January;
        uint8 day = 0;    // [1-31]
        uint16 year = 0;

        Day weekDay = Day::Monday;
        uint16 yearDay = 0;   // [0, 365]

    };

}
