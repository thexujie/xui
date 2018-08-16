#include "stdafx.h"
#include "datetime.h"

#define _CRT_NO_TIME_T
#include <time.h>
#include <sys/timeb.h>

namespace core
{
    static int32_t _curr_timezone()
    {
        long timezone = 0;
        _get_timezone(&timezone);
        return (int32_t)timezone;
    }

    datetime::datetime()
    {
        
    }

    datetime::datetime(std::chrono::milliseconds msec)
    {
        timezone = _curr_timezone();
        int64_t m = msec.count();
        __time64_t sec = m / 1000 + timezone;
        tm ltime;
        _gmtime64_s(&ltime, &sec);
        year = ltime.tm_year + 1900;
        month = ltime.tm_mon + 1;
        day = ltime.tm_mday;
        hour = ltime.tm_hour;
        minute = ltime.tm_min;
        second = ltime.tm_sec;
        millisecond = static_cast<int32_t>(m % 1000);
        microsecond = 0;
        nanosecond = 0;
    }

    datetime::datetime(std::chrono::microseconds mcsec)
    {
        timezone = _curr_timezone();
        int64_t cnt = mcsec.count();
        __time64_t sec = cnt / 1000000 + timezone;
        tm ltime;
        _gmtime64_s(&ltime, &sec);
        year = ltime.tm_year + 1900;
        month = ltime.tm_mon + 1;
        day = ltime.tm_mday;
        hour = ltime.tm_hour;
        minute = ltime.tm_min;
        second = ltime.tm_sec;
        millisecond = static_cast<int32_t>((cnt / 1000) % 1000);
        microsecond = cnt % 1000;
        nanosecond = 0;
    }

    datetime::datetime(std::chrono::nanoseconds nsec)
    {
        timezone = _curr_timezone();
        int64_t cnt = nsec.count();
        __time64_t sec = cnt / 1000000000 + timezone;
        tm ltime;
        _gmtime64_s(&ltime, &sec);
        year = ltime.tm_year + 1900;
        month = ltime.tm_mon + 1;
        day = ltime.tm_mday;
        hour = ltime.tm_hour;
        minute = ltime.tm_min;
        second = ltime.tm_sec;
        millisecond = static_cast<int32_t>((cnt / 1000000) % 1000);
        microsecond = static_cast<int32_t>((cnt / 1000) % 1000);
        nanosecond = static_cast<int32_t>((cnt / 1) % 1000);
    }


    datetime::datetime(int32_t _year, int32_t _month, int32_t _day, int32_t _hour, int32_t _minute, int32_t _second, int32_t _millisecond, int32_t _microsecond, int32_t _nanosecond) :
        year(_year), month(_month), day(_day), hour(_hour), minute(_minute), second(_second), millisecond(_millisecond), microsecond(_microsecond), nanosecond(_nanosecond), timezone(_curr_timezone())
    {
        
    }

    datetime::datetime(const datetime & another)
    {
        year = another.year;
        month = another.month;
        day = another.day;
        hour = another.hour;
        minute = another.minute;
        second = another.second;
        millisecond = another.millisecond;
        microsecond = another.microsecond;
        nanosecond = another.nanosecond;
        timezone = another.timezone;
    }

    datetime & datetime::operator = (const datetime & another)
    {
        year = another.year;
        month = another.month;
        day = another.day;
        hour = another.hour;
        minute = another.minute;
        second = another.second;
        millisecond = another.millisecond;
        microsecond = another.microsecond;
        nanosecond = another.nanosecond;
        timezone = another.timezone;
        return *this;
    }

    int64_t datetime::msecs() const
    {
        tm ltime = {};
        ltime.tm_year = static_cast<int32_t>(year - 1900);
        ltime.tm_mon = static_cast<int32_t>(month - 1);
        ltime.tm_mday = static_cast<int32_t>(day);
        ltime.tm_hour = static_cast<int32_t>(hour);
        ltime.tm_min = static_cast<int32_t>(minute);
        ltime.tm_sec = static_cast<int32_t>(second);
        __time64_t sec = _mkgmtime64(&ltime);
        return (sec - timezone) * 1000 + millisecond;
    }

    int64_t datetime::mcsecs() const
    {
        return msecs() * 1000 + microsecond;
    }

    int64_t datetime::nsecs() const
    {
        return nsecs() * 1000 + nanosecond;
    }

    bool datetime::is_leapyear(int32_t year)
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    enum dayweek datetime::dayweek(int32_t year, int32_t month, int32_t day)
    {
        if (month < 1 || 12 < month)
            return dayweek_monday;

        static int32_t DAY[] = { 6, 2, 1, 4, 6, 2, 4, 0, 3, 5, 1, 3 };
        year -= month < 3;
        return static_cast<enum dayweek>((year + year / 4 - year / 100 + year / 400 + DAY[month - 1] + day) % 7);
    }

    int32_t datetime::dayyear(int32_t year, int32_t month, int32_t day)
    {
        if (month < 1 || 12 < month)
            return 0;

        static int32_t DAY[] = { 1, 0, 2, 0, 3, 0, 4, 5, 0, 6, 0, 7 };
        int32_t temp = (month - 1) * 30 + DAY[month - 1] + day;
        if (month > 2)
            temp -= is_leapyear(year) ? 1 : 2;
        return temp;
    }

    std::chrono::nanoseconds datetime::system()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    }

    std::chrono::nanoseconds datetime::steady()
    {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    }

    std::chrono::nanoseconds datetime::high_resolution()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    }

    float64_t datetime::system_s()
    {
        auto now = std::chrono::system_clock::now();
        auto msecs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return msecs / 1.0;
    }

    float64_t datetime::steady_s()
    {
        auto now = std::chrono::steady_clock::now();
        auto msecs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return msecs / 1.0;
    }

    float64_t datetime::high_resolution_s()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto msecs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return msecs / 1.0;
    }

    std::chrono::seconds datetime::now_s()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    }

    std::chrono::milliseconds datetime::now_ms()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    }

    std::chrono::microseconds datetime::now_mcs()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    }

    std::chrono::nanoseconds datetime::now_ns()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    }

    datetime datetime::now()
    {
        return datetime(now_ms());
    }
}
