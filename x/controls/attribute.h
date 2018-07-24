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
}
