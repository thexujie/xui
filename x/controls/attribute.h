#pragma once

namespace controls
{
    template<typename T>
    class attribute
    {
    public:
        attribute() : value() {}
        ~attribute() {}

        T & get() { return value; }
        const T & get() const{ return value; }
        void set(const T & value_) { value = value_; }

        attribute & operator = (const T & value_)
        {
            _na = false;
            value = value_;
            return *this;
        }

        operator T & () { return value; }
        operator const T & () const { return value; }
        operator bool() const { return !_na; }
        bool aviliable() const { return !_na; }

        bool operator == (const T & value_) const { return value == value_; }
        bool operator != (const T & value_) const { return value != value_; }
        bool operator > (const T & value_) const { return value > value_; }
        bool operator < (const T & value_) const { return value < value_; }
        bool operator >= (const T & value_) const { return value >= value_; }
        bool operator <= (const T & value_) const { return value <= value_; }


    public:
        bool _na = true;
        T value;
    };

    template<typename T>
    struct unit_value
    {
        constexpr unit_value() :value() {}
        constexpr unit_value(const T & value_, core::unit unit_) : value(value_), unit(unit_) {}

        unit_value<T> operator + ( const unit_value<T> & another) const
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

        T value;
        core::unit unit = core::unit::px;
    };

    template<typename T>
    constexpr unit_value<T> operator * (const T & val, core::unit unit)
    {
        return unit_value<T>(val, unit);
    }

    enum class position_origin
    {
        // 默认位置
        layout = 0,
        // 相对于 parent 的位置
        absolute,
        // 相对于 scene 的位置
        fixed,
        //相对与 layout 的位置
        relative,
        // 同 relative，如果超出 scene 则相对于 fixed
        sticky,
    };

    struct layout_size
    {
        void set_absolutive(const unit_value<core::si32f> & size) { absolutive = size; }

        enum class mode
        {
            not_set = 0,
            relative,
            absolutive,
            percentage,
        };

        mode mode = mode::not_set;
        unit_value<core::si32f> absolutive;
    };

    enum class image_repeat
    {
        repeat = 0,
        repeat_x,
        repeat_y,
        repeat_none,
        inherit,
    };

    enum class control_box
    {
        // 边框方框
        border_box = 0,
        // 内框方框
        padding_box,
        // 内容方框
        content_box,
    };

    enum class border_style
    {
        none = 0,
        hidden,
        dotted,
        dashed,
        solid,
        double_,
        groove,
        ridge,
        inset,
        outset,
        inherit,
    };

    struct conntrol_border
    {
        unit_value<float32_t> width;
        border_style style;
        core::color32 color;
    };
}
