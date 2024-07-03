//
// Created by Nikolay Kanchevski on 25.09.23.
//

#include "Time.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    TimeStep::TimeStep(const float64 givenDuration, const TimeStepUnit unit)
    {
        switch (unit)
        {
            case TimeStepUnit::Nanoseconds:
            {
                duration = givenDuration * 0.000'001;
                break;
            }
            case TimeStepUnit::Microseconds:
            {
                duration = givenDuration * 0.001;
                break;
            }
            case TimeStepUnit::Milliseconds:
            {
                duration = givenDuration * 1.0;
                break;
            }
            case TimeStepUnit::Seconds:
            {
                duration = givenDuration * 1'000;
                break;
            }
        }
    }

    /* --- CONSTRUCTORS --- */

    TimePoint::TimePoint(const std::chrono::system_clock::time_point timePoint)
        : timePoint(timePoint)
    {

    }

    TimePoint TimePoint::Now()
    {
        return TimePoint(std::chrono::system_clock::now());
    }

    /* --- OPERATORS --- */

    TimePoint TimePoint::operator-(const TimeStep step) const
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint - std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(step.GetDurationInMilliseconds())))));
    }

    TimePoint TimePoint::operator+(const TimeStep step) const
    {
        return TimePoint(std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(timePoint + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(step.GetDurationInMilliseconds())))));
    }

    TimeStep TimePoint::operator-(const TimePoint other) const
    {
        return TimeStep(static_cast<float64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float64>(timePoint - other.timePoint)).count()));
    }

    /* --- CONSTRUCTORS --- */

    Date::Date(const DateCreateInfo &createInfo)
    {
        const time_t rawTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock, std::chrono::days>(std::chrono::sys_days(std::chrono::year_month_day(std::chrono::year(createInfo.year), std::chrono::month(static_cast<uint8>(createInfo.month) + 1), std::chrono::day(createInfo.day)))));
        const tm &localTime = *localtime(&rawTime);

        second = localTime.tm_sec;
        minute = localTime.tm_min;
        hour = localTime.tm_hour;
        month = static_cast<Month>(localTime.tm_mon);
        day = localTime.tm_mday;
        year = localTime.tm_year + 1900;

        weekDay = (localTime.tm_wday == 0) ? Day::Sunday : static_cast<Day>(localTime.tm_wday - 1);
        yearDay = localTime.tm_yday + 1;
    }

    Date::Date(const TimePoint &point)
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

    bool Date::operator<(const Date other) const
    {
        if (year != other.year)         return year < other.year;
        if (yearDay != other.yearDay)   return yearDay < other.yearDay;
        if (hour != other.hour)         return hour < other.hour;
        if (minute != other.minute)     return minute < other.minute;
        return second < other.second;
    }

    bool Date::operator>(const Date other) const
    {
        return !(*this < other) && !(*this == other);
    }

    bool Date::operator<=(const Date other) const
    {
        return *this < other || *this == other;
    }

    bool Date::operator>=(const Date other) const
    {
        return *this > other || *this == other;
    }

    bool Date::operator==(const Date other) const
    {
        return year == other.year && yearDay == other.yearDay && hour == other.hour && minute == other.minute && second == other.second;
    }

    bool Date::operator!=(const Date other) const
    {
        return !(*this == other);
    }
}
