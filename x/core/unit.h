#pragma once

namespace core
{
    enum class unit
    {
        // 像素（72dpi）
        px = 0,
        // 标准字符代销
        em,
        // 点
        pt,
        //百分比
        per,
        //真实像素
        dot,
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

        bool nan() const { return std::isnan(value); }

        T value;
        core::unit unit = core::unit::px;
    };
    inline unit_value<float32_t> unit_px(float32_t val) { return unit_value<float32_t>(val, core::unit::px); }
    inline unit_value<float32_t> unit_em(float32_t val) { return unit_value<float32_t>(val, core::unit::em); }
    inline unit_value<float32_t> unit_pt(float32_t val) { return unit_value<float32_t>(val, core::unit::pt); }
    inline unit_value<float32_t> unit_per(float32_t val) { return unit_value<float32_t>(val, core::unit::per); }
    inline unit_value<float32_t> unit_dot(float32_t val) { return unit_value<float32_t>(val, core::unit::dot); }

    inline static const unit_value<float32_t> auto_value = unit_value<float32_t>(std::nan("0"), core::unit::px);
    namespace unit_literals
    {

        constexpr unit_value<float32_t> operator "" _px(unsigned long long  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::px);
        }

        constexpr unit_value<float32_t> operator "" _em(unsigned long long  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::em);
        }

        constexpr unit_value<float32_t> operator "" _pt(unsigned long long  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::pt);
        }

        constexpr unit_value<float32_t> operator "" _per(unsigned long long  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::per);
        }

        constexpr unit_value<float32_t> operator "" _dot(unsigned long long  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::dot);
        }

        constexpr unit_value<float32_t> operator "" _px(long double  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::px);
        }

        constexpr unit_value<float32_t> operator "" _em(long double  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::em);
        }

        constexpr unit_value<float32_t> operator "" _pt(long double  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::pt);
        }

        constexpr unit_value<float32_t> operator "" _per(long double  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::per);
        }

        constexpr unit_value<float32_t> operator "" _dot(long double  val)
        {
            return unit_value<float32_t>(static_cast<float32_t>(val), core::unit::dot);
        }
    }
}

