//
// Created by Nikolay Kanchevski on 15.11.26.
//

#include "TimePoint.h"

namespace Sierra
{

    namespace
    {
        tm ToLocalTime(const std::chrono::system_clock::time_point timePoint) noexcept
        {
            const time_t rawTime = std::chrono::system_clock::to_time_t(timePoint);

            tm localTime = { };
            #if SR_PLATFORM_WINDOWS
                localtime_s(&localTime, &rawTime);
            #else
                localtime_r(&rawTime, &localTime);
            #endif

            return localTime;
        }

        std::chrono::system_clock::duration ToClockDuration(const TimeStep step) noexcept
        {
            return std::chrono::round<std::chrono::system_clock::duration>(std::chrono::duration<float64, std::milli>(step.GetDuration(TimeStepUnit::Milliseconds)));
        }
    }

    /* --- CONSTRUCTORS --- */

    TimePoint::TimePoint(const std::chrono::system_clock::time_point timePoint) noexcept
        : timePoint(timePoint)
    {

    }

    TimePoint TimePoint::Now() noexcept
    {
        return TimePoint(std::chrono::system_clock::now());
    }

    /* --- GETTER METHODS --- */

    uint8 TimePoint::GetSecond() const noexcept
    {
        return static_cast<uint8>(ToLocalTime(timePoint).tm_sec);
    }

    uint8 TimePoint::GetMinute() const noexcept
    {
        return static_cast<uint8>(ToLocalTime(timePoint).tm_min);
    }

    uint8 TimePoint::GetHour() const noexcept
    {
        return static_cast<uint8>(ToLocalTime(timePoint).tm_hour);
    }

    Month TimePoint::GetMonth() const noexcept
    {
        return static_cast<Month>(ToLocalTime(timePoint).tm_mon);
    }

    uint8 TimePoint::GetDay() const noexcept
    {
        return static_cast<uint8>(ToLocalTime(timePoint).tm_mday);
    }

    uint16 TimePoint::GetYear() const noexcept
    {
        return static_cast<uint16>(ToLocalTime(timePoint).tm_year + 1900);
    }

    Day TimePoint::GetWeekDay() const noexcept
    {
        const int32 weekDay = ToLocalTime(timePoint).tm_wday;
        return weekDay == 0 ? Day::Sunday : static_cast<Day>(weekDay - 1);
    }

    uint16 TimePoint::GetYearDay() const noexcept
    {
        return static_cast<uint16>(ToLocalTime(timePoint).tm_yday + 1);
    }

    /* --- OPERATORS --- */

    TimePoint TimePoint::operator+(const TimeStep step) const noexcept
    {
        return TimePoint(timePoint + ToClockDuration(step));
    }

    TimePoint TimePoint::operator-(const TimeStep step) const noexcept
    {
        return TimePoint(timePoint - ToClockDuration(step));
    }

    TimeStep TimePoint::operator-(const TimePoint other) const noexcept
    {
        return TimeStep(std::chrono::duration<float64, std::milli>(timePoint - other.timePoint).count(), TimeStepUnit::Milliseconds);
    }

    /* --- CONVERSIONS --- */

    std::string TimePointToString(const TimePoint timePoint) noexcept
    {
        return SR_FORMAT(
            "{0:02}/{1:02}/{2:04} {3:02}:{4:02}:{5:02}",
            static_cast<uint32>(timePoint.GetDay()), static_cast<uint32>(timePoint.GetMonth()) + 1, static_cast<uint32>(timePoint.GetYear()),
            static_cast<uint32>(timePoint.GetHour()), static_cast<uint32>(timePoint.GetMinute()), static_cast<uint32>(timePoint.GetSecond())
        );
    }

}
