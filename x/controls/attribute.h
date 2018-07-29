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
        // Ĭ��λ��
        layout = 0,
        // ����� parent ��λ��
        parent,
        // ����� scene ��λ��
        scene,
        // ����� view ��λ��
        view,
        // ͬ layout��������� scene ������� view
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
        // �߿򷽿�
        border_box = 0,
        // �ڿ򷽿�
        padding_box,
        // ���ݷ���
        content_box,
    };
}
