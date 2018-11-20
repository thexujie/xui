#pragma once

namespace core
{
    enum class unit
    {
        // 像素（72dpi）
        px = 0,
        // 标准字符高度
        em,
		// 当前字符高度
		ft,
        // 点
        pt,
        //真实像素
        dot,
        //百分比
        per,
    };

    template<typename T>
    struct unit_value
    {
        constexpr unit_value() :value() {}
        constexpr unit_value(const T & value_, core::unit unit_) : value(value_), unit(unit_) {}

        unit_value<T> operator + (const unit_value<T> & another) const
        {
            if (unit != another.unit)
                throw core::error_bad_format;

            return unit_value<T>(value + another.value, unit);
        }

        unit_value<T> operator - (const unit_value<T> & another) const
        {
            if (unit != another.unit)
                throw core::error_bad_format;

            return unit_value<T>(value - another.value, unit);
        }

        bool operator == (const unit_value<T> & another) const { return value == another.value; }
        bool operator != (const unit_value<T> & another) const { return value != another.value; }

        unit_value<T> operator * (const T & ratio) const { return unit_value<T>(value * ratio, unit); }
        unit_value<T> operator / (const T & ratio) const { return unit_value<T>(value / ratio, unit); }
        unit_value<T> & operator *= (const T & ratio) const { value *= ratio;  return *this; }
        unit_value<T> & operator /= (const T & ratio) const { value *= ratio;  return *this; }

        bool avi() const { return !std::isnan(value); }
        bool nan() const { return std::isnan(value); }

        bool px() const { return unit == unit::px && !std::isnan(value); }
		bool em() const { return unit == unit::em && !std::isnan(value); }
		bool ft() const { return unit == unit::ft && !std::isnan(value); }
        bool pt() const { return unit == unit::pt && !std::isnan(value); }
        bool dot() const { return unit == unit::dot && !std::isnan(value); }
        bool per() const { return unit == unit::per && !std::isnan(value); }

        T  value;
        core::unit unit = core::unit::px;
    };

    typedef unit_value<float32_t> dimenf;

    inline dimenf unit_px(float32_t val) { return dimenf(val, core::unit::px); }
	inline dimenf unit_em(float32_t val) { return dimenf(val, core::unit::em); }
	inline dimenf unit_ft(float32_t val) { return dimenf(val, core::unit::ft); }
    inline dimenf unit_pt(float32_t val) { return dimenf(val, core::unit::pt); }
    inline dimenf unit_dot(float32_t val) { return dimenf(val, core::unit::dot); }

    inline static const dimenf auto_value = dimenf(std::nanf("0"), core::unit::px);
    namespace unit_literals
    {
        constexpr dimenf operator "" _px(unsigned long long  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::px);
        }

        constexpr dimenf operator "" _em(unsigned long long  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::em);
        }

		constexpr dimenf operator "" _ft(unsigned long long  val)
		{
			return dimenf(static_cast<float32_t>(val), core::unit::ft);
		}

        constexpr dimenf operator "" _pt(unsigned long long  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::pt);
        }

        constexpr dimenf operator "" _dot(unsigned long long  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::dot);
        }

        constexpr dimenf operator "" _per(unsigned long long  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::per);
        }

        constexpr dimenf operator "" _px(long double  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::px);
        }

        constexpr dimenf operator "" _em(long double  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::em);
        }

		constexpr dimenf operator "" _ft(long double  val)
		{
			return dimenf(static_cast<float32_t>(val), core::unit::ft);
		}

        constexpr dimenf operator "" _pt(long double  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::pt);
        }

        constexpr dimenf operator "" _dot(long double  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::dot);
        }

        constexpr dimenf operator "" _per(long double  val)
        {
            return dimenf(static_cast<float32_t>(val), core::unit::per);
        }
    }

    struct unit_name
    {
        core::unit unit;
        const char * name;
    };

    const unit_name unit_names[] =
    {
        { core::unit::px, "px" },
        { core::unit::em, "em" },
        { core::unit::ft, "ft" },
        { core::unit::pt, "pt" },
        { core::unit::dot, "dot" },
        { core::unit::per, "per" },
    };
}

