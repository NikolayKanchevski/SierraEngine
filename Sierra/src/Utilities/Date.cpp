//
// Created by Nikolay Kanchevski on 24.09.24.
//

#include "Date.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Date::Date(const DateCreateInfo& createInfo) noexcept
    {
        const time_t rawTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock, std::chrono::days>(std::chrono::sys_days(std::chrono::year_month_day(std::chrono::year(createInfo.year), std::chrono::month(static_cast<uint8>(createInfo.month) + 1), std::chrono::day(createInfo.day)))));
        const tm& localTime = *localtime(&rawTime);

        second = createInfo.second;
        minute = createInfo.minute;
        hour = createInfo.hour;
        month = createInfo.month;
        day = createInfo.day;
        year = createInfo.year;

        weekDay = (localTime.tm_wday == 0) ? Day::Sunday : static_cast<Day>(localTime.tm_wday - 1);
        yearDay = localTime.tm_yday + 1;
    }

    Date::Date(const TimePoint& point) noexcept
    {
        const time_t rawTime = std::chrono::system_clock::to_time_t(point.timePoint);
        const tm localTime = *localtime(&rawTime);

        second = localTime.tm_sec;
        minute = localTime.tm_min;
        hour = localTime.tm_hour;
        month = static_cast<Month>(localTime.tm_mon);
        day = localTime.tm_mday;
        year = localTime.tm_year + 1900;

        weekDay = (localTime.tm_wday == 0) ? Day::Sunday : static_cast<Day>(localTime.tm_wday - 1);
        yearDay = localTime.tm_yday + 1;
    }

    /* --- OPERATORS --- */

    bool Date::operator<(const Date other) const noexcept
    {
        if (year != other.year)         return year < other.year;
        if (yearDay != other.yearDay)   return yearDay < other.yearDay;
        if (hour != other.hour)         return hour < other.hour;
        if (minute != other.minute)     return minute < other.minute;
        return second < other.second;
    }

    bool Date::operator>(const Date other) const noexcept
    {
        return !(*this < other) && !(*this == other);
    }

    bool Date::operator<=(const Date other) const noexcept
    {
        return *this < other || *this == other;
    }

    bool Date::operator>=(const Date other) const noexcept
    {
        return *this > other || *this == other;
    }

}