//
// Created by Nikolay Kanchevski on 15.11.26.
//

#pragma once

#include "TimeStep.h"

namespace Sierra
{


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

    class SIERRA_API TimePoint final
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimePoint() noexcept = default;

        explicit TimePoint(std::chrono::system_clock::time_point timePoint) noexcept;
        static TimePoint Now() noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint8 GetSecond() const noexcept;
        [[nodiscard]] uint8 GetMinute() const noexcept;
        [[nodiscard]] uint8 GetHour() const noexcept;
        [[nodiscard]] Month GetMonth() const noexcept;
        [[nodiscard]] uint8 GetDay() const noexcept;
        [[nodiscard]] uint16 GetYear() const noexcept;

        [[nodiscard]] Day GetWeekDay() const noexcept;
        [[nodiscard]] uint16 GetYearDay() const noexcept;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const TimePoint other) const noexcept { return timePoint == other.timePoint; }
        [[nodiscard]] bool operator!=(const TimePoint other) const noexcept { return !(*this == other); }

        [[nodiscard]] bool operator<(const TimePoint other) const noexcept { return timePoint < other.timePoint; }
        [[nodiscard]] bool operator>(const TimePoint other) const noexcept { return timePoint > other.timePoint; }
        [[nodiscard]] bool operator<=(const TimePoint other) const noexcept { return timePoint <= other.timePoint; }
        [[nodiscard]] bool operator>=(const TimePoint other) const noexcept { return timePoint >= other.timePoint; }

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
        std::chrono::system_clock::time_point timePoint;

    };

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API std::string TimePointToString(TimePoint timePoint) noexcept;

}