#pragma once

namespace core
{
    enum dayweek
    {
        //! 星期一。
        dayweek_monday = 0,
        //! 星期二。
        dayweek_tuesday,
        //! 星期三。
        dayweek_wednesday,
        //! 星期四。
        dayweek_thursday,
        //! 星期五。
        dayweek_friday,
        //! 星期六。
        dayweek_saturday,
        //! 星期日。
        dayweek_sunday
    };

    class datetime
    {
    public:
        datetime();
        datetime(std::chrono::milliseconds msec);
        datetime(std::chrono::microseconds mcsec);
        datetime(std::chrono::nanoseconds nsec);
        datetime(int32_t _year, int32_t _month, int32_t _day, int32_t _hour, int32_t _minute, int32_t _second, int32_t _millisecond, int32_t _microsecond, int32_t _nanosecond);
        datetime(const datetime & another);
        datetime & operator =(const datetime & another);
        bool operator ==(const datetime & another) const{ return nsecs() == another.nsecs();   }
        bool operator !=(const datetime & another) const{ return !operator==(another);   }
        int64_t msecs() const;
        int64_t mcsecs() const;
        int64_t nsecs() const;

        //! 年[-, -]
        int32_t year;
        //! 月 [1, 12]
        int32_t month;
        //! 日 [1, 31]
        int32_t day;
        //! 时 [0, 23]
        int32_t hour;
        //! 分 [0, 59]
        int32_t minute;
        //! 秒 [0, 59]
        int32_t second;
        //! 毫秒 [0, 999]
        int32_t millisecond;
        //! 微秒 [0, 999]
        int32_t microsecond;
        //! 纳秒 [0, 999]
        int32_t nanosecond;
        //! 纳秒 [0, 999]
        int32_t timezone;

        static bool is_leapyear(int32_t year);
        static enum dayweek dayweek(int32_t year, int32_t month, int32_t day);
        static 	int32_t dayyear(int32_t year, int32_t month, int32_t day);

        static datetime now();
		static float64_t system();
		static std::chrono::nanoseconds system_ns();
		static float64_t steady();
		static std::chrono::nanoseconds steady_ns();
		static float64_t high_resolution();
		static std::chrono::nanoseconds high_resolution_ns();
    };
}
