//
// Created by Nikolay Kanchevski on 24.09.24.
//

#pragma once

#include "Time.h"

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

    struct DateCreateInfo
    {
        uint8 second = 0;
        uint8 minute = 0;
        uint8 hour = 0;
        Month month = Month::January;
        uint8 day = 1;
        uint16 year = 0;
    };

    class SIERRA_API Date final
    {
    public:
        /* --- CONSTRUCTORS --- */
        Date() noexcept = default;
        explicit Date(const DateCreateInfo& createInfo) noexcept;
        explicit Date(const TimePoint& point) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint8 GetSecond() const noexcept { return second; }
        [[nodiscard]] uint8 GetMinute() const noexcept { return minute; }
        [[nodiscard]] uint8 GetHour() const noexcept { return hour; }
        [[nodiscard]] Month GetMonth() const noexcept { return month; }
        [[nodiscard]] uint8 GetDay() const noexcept { return day; }
        [[nodiscard]] uint16 GetYear() const noexcept { return year; }

        [[nodiscard]] Day GetWeekDay() const noexcept { return weekDay; }
        [[nodiscard]] uint16 GetYearDay() const noexcept { return yearDay; }

        /* --- COPY SEMANTICS --- */
        Date(const Date&) noexcept = default;
        Date& operator=(const Date&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Date(Date&&) noexcept = default;
        Date& operator=(Date&&) noexcept = default;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const Date other) const noexcept { return year == other.year && yearDay == other.yearDay && hour == other.hour && minute == other.minute && second == other.second; }
        [[nodiscard]] bool operator!=(const Date other) const noexcept { return !(*this == other); }

        [[nodiscard]] bool operator<(Date other) const noexcept;
        [[nodiscard]] bool operator>(Date other) const noexcept;
        [[nodiscard]] bool operator<=(Date other) const noexcept;
        [[nodiscard]] bool operator>=(Date other) const noexcept;

        /* --- DESTRUCTORS --- */
        ~Date() noexcept = default;

    private:
        uint8 second = 0;
        uint8 minute = 0;
        uint8 hour = 0;
        Month month = Month::January;
        uint8 day = 0;
        uint16 year = 0;

        Day weekDay = Day::Monday;
        uint16 yearDay = 0;

    };

}