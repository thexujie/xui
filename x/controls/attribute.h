#pragma once

namespace controls
{
    template<typename T>
    class attribute
    {
    public:
        attribute() : _value() {}
        ~attribute() {}

        T & get() { return _value; }
        const T & get() const{ return _value; }
        void set(const T & value) { _value = value; }

        attribute & operator = (const T & value)
        {
            _value = value;
            return *this;
        }

        bool operator == (const T & value) const { return _value == value; }
        bool operator != (const T & value) const { return _value != value; }
        bool operator > (const T & value) const { return _value > value; }
        bool operator < (const T & value) const { return _value < value; }
        bool operator >= (const T & value) const { return _value >= value; }
        bool operator <= (const T & value) const { return _value <= value; }

    private:
        T _value;
    };

    template<typename T>
    struct unit_value
    {
        constexpr unit_value() :value() {}
        constexpr unit_value(const T & value_, core::unit unit_) : value(value_), unit(unit_) {}
        bool set() const { return unit == core::unit::na; }
        operator bool() const { return unit == core::unit::na; }

        T value;
        core::unit unit = core::unit::na;
    };

    template<typename T>
    constexpr unit_value<T> operator * (const T & val, core::unit unit)
    {
        return unit_value<T>(val, unit);
    }


    struct layout_position
    {
        void set_relative(core::align pos) { relative = pos; mode = mode::relative; }
        void set_absolutive(const unit_value<core::pt32f> & pos) { absolutive = pos;  mode = mode::absolutive; }

        enum class mode
        {
            relative,
            absolutive,
            percentage,
        };

        mode mode = mode::relative;
        core::align relative = core::align::leftTop;
        unit_value<core::pt32f> absolutive;
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
