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

        T value;
    private:
        bool _na = true;
    };

    enum class image_fitting
    {
        none = 0,
        scale,
        repeat,
    };

    enum class layout_origin
    {
        // 默认位置
        layout = 0,
        // 相对于 parent 的位置
        parent,
        // 相对于 scene 的位置
        scene,
        // 相对于 view 的位置
        view,
        // 同 layout，如果超出 scene 则相对于 view
        sticky,
    };

    struct layout_size
    {
        void set_absolutive(const core::unit_value<core::si32f> & size) { absolutive = size; }

        enum class mode
        {
            not_set = 0,
            relative,
            absolutive,
            percentage,
        };

        mode mode = mode::not_set;
        core::unit_value<core::si32f> absolutive;
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
}
